#include "all-headers.h"

//--------------------------------------------------------------------------------------------------
//  Check CPU CLOCK
//  CPU clock is defined in makefile.json
//  and is reported in the CPU_MHZ compile time variable (in zSOURCES/base.h)
//--------------------------------------------------------------------------------------------------

#ifndef CPU_MHZ
  #error "CPU_MHZ is not defined"
#elif CPU_MHZ == 25
  // Ok
#elif CPU_MHZ == 125
  // Ok
#else
  #error "Invalid CPU_MHZ value: required value: 25 or 125"
#endif

//--------------------------------------------------------------------------------------------------

// static const uint32_t XOSC_MHZ = 12 ; // Crystal on Raspberry Pi Pico is 12 MHz
static const uint32_t MHZ = 1000 * 1000 ;

//--------------------------------------------------------------------------------------------------
// pll_init
// From Arduino, rp2040/hardware/rp2040/1.1.0/pico-sdk/src/rp2_common/hardware_pll/pll.c
//--------------------------------------------------------------------------------------------------

static void pll_init (pll_hw_t * pll,
                      const uint32_t refdiv,
                      const uint32_t vco_freq,
                      const uint32_t post_div1,
                      const uint32_t post_div2) {
//--- Turn off PLL in case it is already running
  pll->pwr = 0xFFFFFFFF ; // Page 256
  pll->fbdiv_int = 0 ; // Page 257

  const uint32_t ref_mhz = XOSC_MHZ / refdiv ;
  pll->cs = refdiv ; // Page 256

//--- What are we multiplying the reference clock by to get the vco freq
// (The regs are called div, because you divide the vco output and compare it to the refclk)
  const uint32_t fbdiv = vco_freq / (ref_mhz * MHZ) ;

//--- Assertions
//     // fbdiv
//     assert(fbdiv >= 16 && fbdiv <= 320);
//
//     // Check divider ranges
//     assert((post_div1 >= 1 && post_div1 <= 7) && (post_div2 >= 1 && post_div2 <= 7));
//
//     // post_div1 should be >= post_div2
//     // from appnote page 11
//     // postdiv1 is designed to operate with a higher input frequency
//     // than postdiv2
//     assert(post_div2 <= post_div1);
//
// /// \tag::pll_init_finish[]
//     // Check that reference frequency is no greater than vco / 16
//     assert(ref_mhz <= (vco_freq / 16));

//--- Put calculated value into feedback divider
  pll->fbdiv_int = fbdiv ;

//--- Turn on PLL
  const uint32_t power = PLL_PWR_PD_BITS | // Main power
                         PLL_PWR_VCOPD_BITS ; // VCO Power
  pll->pwr &= ~ power ;

//--- Wait for PLL to lock
  while ((pll->cs & PLL_CS_LOCK_BITS) == 0) {}
//-- Set up post dividers - div1 feeds into div2 so if div1 is 5 and div2 is 2
//   then you get a divide by 10
  const uint32_t pdiv = (post_div1 << PLL_PRIM_POSTDIV1_LSB) |
                        (post_div2 << PLL_PRIM_POSTDIV2_LSB);
  pll->prim = pdiv;

//--- Turn on post divider
  pll->pwr &= ~ PLL_PWR_POSTDIVPD_BITS ;
}

//----------------------------------------------------------------------------------------------
// xosc_init
// From Arduino, rp2040/hardware/rp2040/1.1.0/pico-sdk/src/rp2_common/hardware_xosc/xosc.c
//--------------------------------------------------------------------------------------------------

static void xosc_init (void) {
//--- Assumes 1-15 MHz input
//     assert(XOSC_MHZ <= 15);
  xosc_hw->ctrl = XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ;

//--- Set xosc startup delay
  const uint32_t startup_delay = (((12 * MHZ) / 1000) + 128) / 256;
  xosc_hw->startup = startup_delay ; // page 244

//--- Set the enable bit now that we have set freq range and startup delay
  xosc_hw->ctrl |= 0xFAB << 12 ; // Page 243
//     hw_set_bits(&xosc_hw->ctrl, XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB);

//--- Wait for XOSC to be stable
  while(!(xosc_hw->status & XOSC_STATUS_STABLE_BITS)) {}
}

//--------------------------------------------------------------------------------------------------
// ClockIndex
// From Arduino, rp2040/hardware/rp2040/1.1.0/pico-sdk/src/rp2040/hardware_structs/
//               include/hardware/structs/clocks.h
//--------------------------------------------------------------------------------------------------

enum class ClockIndex : uint32_t {
  clk_gpout0 = 0,  // GPIO Muxing 0
  clk_gpout1 = 1,  // GPIO Muxing 1
  clk_gpout2 = 2,  // GPIO Muxing 2
  clk_gpout3 = 3,  // GPIO Muxing 3
  clk_ref    = 4,  // Watchdog and timers reference clock
  clk_sys    = 5,  // Processors, bus fabric, memory, memory mapped registers
  clk_peri   = 6,  // Peripheral clock for UART and SPI
  clk_usb    = 7,  // USB clock
  clk_adc    = 8,  // ADC clock
  clk_rtc    = 9   // Real time clock
//  CLK_COUNT
} ;

//--------------------------------------------------------------------------------------------------
// clock_configure
// From Arduino, rp2040/hardware/rp2040/1.1.0/pico-sdk/src/rp2_common/hardware_clocks/clocks.c
//--------------------------------------------------------------------------------------------------

static void clock_configure (const ClockIndex inClock,
                             const uint32_t src,
                             const uint32_t auxsrc,
                             const uint32_t src_freq,
                             const uint32_t freq) {
//     uint32_t div;
//
//     assert(src_freq >= freq);
//
//     if (freq > src_freq)
//         return false;
//
//--------------------- Div register is 24.8 int.frac divider so multiply by 2^8 (left shift by 8)
  const uint32_t div = uint32_t ((uint64_t (src_freq) << 8) / freq) ;

//--------------------- Clock register address
  const uint32_t clockIndex = uint32_t (inClock) ;
  clock_hw_t * clock = &clocks_hw->clk [clockIndex] ;

//--------------------- If increasing divisor, set divisor before source. Otherwise set source
// before divisor. This avoids a momentary overspeed when e.g. switching
// to a faster source and increasing divisor to compensate.
  if (div > clock->div) {
    clock->div = div ;
  }

//--------------------- If switching a glitchless slice (ref or sys) to an aux source, switch
  const bool has_glitchless_mux =
                    (inClock == ClockIndex::clk_sys) || (inClock == ClockIndex::clk_ref) ;
//   static inline bool has_glitchless_mux(enum clock_index clk_index) {
//     return clk_index == clk_sys || clk_index == clk_ref;
//   }

//--------------------- If switching a glitchless slice (ref or sys) to an aux source, switch
// away from aux *first* to avoid passing glitches when changing aux mux.
// Assume (!!!) glitchless source 0 is no faster than the aux source.
//   const uint32_t CLOCKS_CLK_REF_CTRL_SRC_BITS = 0x00000003 ;
  if (has_glitchless_mux && (src == CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX)) {
    clock->ctrl &= ~ CLOCKS_CLK_REF_CTRL_SRC_BITS ;
    while ((clock->selected & 1) == 0) {}
  }else{
  // If no glitchless mux, cleanly stop the clock to avoid glitches
  // propagating when changing aux mux. Note it would be a really bad idea
  // to do this on one of the glitchless clocks (clk_sys, clk_ref).
    clock->ctrl &= ~ CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS ;
//         if (configured_freq[clk_index] > 0) {
//             // Delay for 3 cycles of the target clock, for ENABLE propagation.
//             // Note XOSC_COUNT is not helpful here because XOSC is not
//             // necessarily running, nor is timer... so, 3 cycles per loop:
//             uint delay_cyc = configured_freq[clk_sys] / configured_freq[clk_index] + 1;
//             asm volatile (
//                 ".syntax unified \n\t"
//                 "1: \n\t"
//                 "subs %0, #1 \n\t"
//                 "bne 1b"
//                 : "+r" (delay_cyc)
//             );
//         }
  }
//  if (has_glitchless_mux(clk_index) && src == CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX) {
//         hw_clear_bits(&clock->ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
//         while (!(clock->selected & 1u))
//             tight_loop_contents();
//     }
//     // If no glitchless mux, cleanly stop the clock to avoid glitches
//     // propagating when changing aux mux. Note it would be a really bad idea
//     // to do this on one of the glitchless clocks (clk_sys, clk_ref).
//     else {
//         hw_clear_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);
//         if (configured_freq[clk_index] > 0) {
//             // Delay for 3 cycles of the target clock, for ENABLE propagation.
//             // Note XOSC_COUNT is not helpful here because XOSC is not
//             // necessarily running, nor is timer... so, 3 cycles per loop:
//             uint delay_cyc = configured_freq[clk_sys] / configured_freq[clk_index] + 1;
//             asm volatile (
//                 ".syntax unified \n\t"
//                 "1: \n\t"
//                 "subs %0, #1 \n\t"
//                 "bne 1b"
//                 : "+r" (delay_cyc)
//             );
//         }
//     }

//--------------------- Set aux mux first, and then glitchless mux if this clock has one
  uint32_t v = (clock->ctrl & ~CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS) ;
  v |= auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB ; // CLOCKS_CLK_REF_CTRL_AUXSRC (auxsrc) ;
  clock->ctrl = v ;
//     hw_write_masked(&clock->ctrl,
//         (auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB),
//         CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS
//     );

//---------------------
  if (has_glitchless_mux) {
    v = (clock->ctrl & ~CLOCKS_CLK_REF_CTRL_SRC_BITS) ;
    v |= src << CLOCKS_CLK_REF_CTRL_SRC_LSB ;
    clock->ctrl = v ;
    while ((clock->selected & (1u << src)) == 0) {}
  }
//     if (has_glitchless_mux(clk_index)) {
//         hw_write_masked(&clock->ctrl,
//             src << CLOCKS_CLK_REF_CTRL_SRC_LSB,
//             CLOCKS_CLK_REF_CTRL_SRC_BITS
//         );
//         while (!(clock->selected & (1u << src)))
//             tight_loop_contents();
//     }

//---------------------
  clock->ctrl |= CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS ;
//     hw_set_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);
//
//--------------------- Now that the source is configured, we can trust that the user-supplied
//                      divisor is a safe value.
  clock->div = div ;
//
//     // Store the configured frequency
//     configured_freq[clk_index] = freq;
//
//     return true;
}

//--------------------------------------------------------------------------------------------------
// clock_init
// From Arduino, rp2040/hardware/rp2040/1.1.0/pico-sdk/src/rp2_common/hardware_clocks/clocks.c
//--------------------------------------------------------------------------------------------------

static void clock_init (void) {
//--------------------- Start tick in watchdog
// watchdog_start_tick(XOSC_MHZ);

//--------------------- Everything is 48MHz on FPGA apart from RTC. Otherwise set to 0 and
//    will be set in clock configure
//     if (running_on_fpga()) {
//         for (uint i = 0; i < CLK_COUNT; i++) {
//             configured_freq[i] = 48 * MHZ;
//         }
//         configured_freq[clk_rtc] = 46875;
//         return;
//     }

//--------------------- Disable resus that may be enabled from previous software (page 231)
  clocks_hw->resus.ctrl = 0 ;

//--------------------- Enable the xosc
  xosc_init () ;

//--------------------- Before we touch PLLs,
//                      switch sys and ref cleanly away from their aux sources.
  clocks_hw->clk[clk_sys].ctrl &= ~ CLOCKS_CLK_SYS_CTRL_SRC_BITS ; // Page 226
//     hw_clear_bits(&clocks_hw->clk[clk_sys].ctrl, CLOCKS_CLK_SYS_CTRL_SRC_BITS);
  while (clocks_hw->clk[clk_sys].selected != 0x1) {} // Page 227

  clocks_hw->clk[clk_ref].ctrl &= ~ CLOCKS_CLK_REF_CTRL_SRC_BITS ; // Page 225
//     hw_clear_bits(&clocks_hw->clk[clk_ref].ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
  while (clocks_hw->clk[clk_ref].selected != 0x1) {} // Page 226

//--------------------- Configure PLLs
//                   REF     FBDIV VCO            POSTDIV
// PLL SYS: 12 / 1 = 12MHz * 125 = 1500MHZ / 6 / 2 = 125MHz
// PLL USB: 12 / 1 = 12MHz * 40  = 480 MHz / 5 / 2 =  48MHz
//--- First reset PLL pheripherals
//     reset_block(RESETS_RESET_PLL_SYS_BITS | RESETS_RESET_PLL_USB_BITS);
//     unreset_block_wait(RESETS_RESET_PLL_SYS_BITS | RESETS_RESET_PLL_USB_BITS);
  const uint32_t resettedPheripheral = RESETS_RESET_PLL_SYS_BITS | RESETS_RESET_PLL_USB_BITS ;
  resets_hw->reset |= resettedPheripheral ;
  resets_hw->reset &= ~resettedPheripheral ;
//--- Wait for reset done
  while ((resettedPheripheral & ~resets_hw->reset_done) != 0) {}
//--- Initialize PLLs
  pll_init (pll_sys_hw, 1, 1500 * MHZ, 6, 2) ;
  pll_init (pll_usb_hw, 1, 480 * MHZ, 5, 2) ;

//--------------------- Configure clocks
//     // CLK_REF = XOSC (12MHz) / 1 = 12MHz
  clock_configure (ClockIndex::clk_ref,
                   CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, // page 225
                   0, // No aux mux
                   12 * MHZ,
                   12 * MHZ) ;
//     clock_configure(clk_ref,
//                     CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC,
//                     0, // No aux mux
//                     12 * MHZ,
//                     12 * MHZ);

// CLK SYS = PLL SYS (125MHz) / 1 = 125MHz
  clock_configure (ClockIndex::clk_sys,
                   CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                   CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                   125 * MHZ,
                   CPU_MHZ * MHZ);
//     clock_configure(clk_sys,
//                     CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
//                     CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
//                     125 * MHZ,
//                     125 * MHZ);

// CLK USB = PLL USB (48MHz) / 1 = 48MHz
  clock_configure (ClockIndex::clk_usb,
                   0, // No GLMUX
                   CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                   48 * MHZ,
                   48 * MHZ);
//     clock_configure(clk_usb,
//                     0, // No GLMUX
//                     CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
//                     48 * MHZ,
//                     48 * MHZ);

// CLK ADC = PLL USB (48MHZ) / 1 = 48MHz
  clock_configure (ClockIndex::clk_adc,
                   0, // No GLMUX
                   CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                   48 * MHZ,
                   48 * MHZ);
//     clock_configure(clk_adc,
//                     0, // No GLMUX
//                     CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
//                     48 * MHZ,
//                     48 * MHZ);

// CLK RTC = PLL USB (48MHz) / 1024 = 46875Hz
  clock_configure (ClockIndex::clk_rtc,
                   0, // No GLMUX
                   CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                   48 * MHZ,
                   46875) ;
//     clock_configure(clk_rtc,
//                     0, // No GLMUX
//                     CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
//                     48 * MHZ,
//                     46875);
/// CLK PERI = clk_sys. Used as reference clock for Peripherals.
//  No dividers so just select and enable
// Normally choose clk_sys or clk_usb
  clock_configure (ClockIndex::clk_peri,
                   0,
                   CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                   125 * MHZ,
                   125 * MHZ);
}

//--------------------------------------------------------------------------------------------------
//  BOOT ROUTINE
// See in Arduino: rp2040/hardware/rp2040/1.1.0/pico-sdk/src/rp2_common/pico_runtime/runtime.c
//--------------------------------------------------------------------------------------------------

void cpu0Phase3Boot (void) {
//------------------------------------ Reset peripherals
//--- Reset peripherals, but 4
  const uint32_t perpheralsNotResetted =
    RESETS_RESET_IO_QSPI_BITS |
    RESETS_RESET_PADS_QSPI_BITS |
    RESETS_RESET_PLL_USB_BITS |
    RESETS_RESET_PLL_SYS_BITS
  ;
  resets_hw->reset = ~ perpheralsNotResetted ;
//--- Remove reset for all peripheral, but adc, rtc, spi0, spi1, uart0, uart1, usbctrl
const uint32_t reset = resets_hw->reset & ~(
    RESETS_RESET_ADC_BITS |
    RESETS_RESET_RTC_BITS |
    RESETS_RESET_SPI0_BITS |
    RESETS_RESET_SPI1_BITS |
    RESETS_RESET_UART0_BITS |
    RESETS_RESET_UART1_BITS |
    RESETS_RESET_USBCTRL_BITS
  ) ;
  resets_hw->reset &= ~reset ;
//--- Wait for reset done
  while ((reset & ~resets_hw->reset_done) != 0) {}
//------------------------------------ Configure clocks
  clock_init () ;
//------------------------------------ Configure VTOR Cortex-M0+ register
  extern uint32_t __vectors_start_cpu_0 ;
  scb_hw->vtor = uint32_t (& __vectors_start_cpu_0) ;
//------------------------------------ Clear '.bss' section
  extern uint32_t __bss_start ;
  extern const uint32_t __bss_end ;
  uint32_t * p = & __bss_start ;
  while (p != & __bss_end) {
    * p = 0 ;
    p ++ ;
  }
//------------------------------------ Copy .data section in RAM
  extern uint32_t __data_start ;
  extern const uint32_t __data_end ;
  extern uint32_t __data_load_start ;
  uint32_t * pSrc = & __data_load_start ;
  uint32_t * pDest = & __data_start ;
  while (pDest != & __data_end) {
    * pDest = * pSrc ;
    pDest ++ ;
    pSrc ++ ;
  }
}

//--------------------------------------------------------------------------------------------------
//   INIT ROUTINE
//--------------------------------------------------------------------------------------------------

void cpu0Phase3Init (void) {
//--------------- Aller exécuter les routines d'initialisation de la section boot.routine.array
  extern void (* __boot_routine_array_start) (void) ;
  extern void (* __boot_routine_array_end) (void) ;
  void (* * ptr) (void) = & __boot_routine_array_start ;
  while (ptr != & __boot_routine_array_end) {
    (* ptr) () ;
    ptr ++ ;
  }
//--------------- Run C++ global variable constructors
  extern void (* __constructor_array_start) (void) ;
  extern void (* __constructor_array_end) (void) ;
  ptr = & __constructor_array_start ;
  while (ptr != & __constructor_array_end) {
    (* ptr) () ;
    ptr ++ ;
  }
//--------------- Aller exécuter les routines d'initialisation de la section init.routine.array
  extern void (* __init_routine_array_start) (void) ;
  extern void (* __init_routine_array_end) (void) ;
  ptr = & __init_routine_array_start ;
  while (ptr != & __init_routine_array_end) {
    (* ptr) () ;
    ptr ++ ;
  }
}

//--------------------------------------------------------------------------------------------------
