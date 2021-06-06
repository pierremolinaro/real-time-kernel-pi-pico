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
  xosc_hw->ctrl = XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ;

//--- Set xosc startup delay
  const uint32_t startup_delay = (((12 * MHZ) / 1000) + 128) / 256;
  xosc_hw->startup = startup_delay ; // page 244

//--- Set the enable bit now that we have set freq range and startup delay
  xosc_hw->ctrl |= 0xFAB << 12 ; // Page 243

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

//--------------------- If switching a glitchless slice (ref or sys) to an aux source, switch
// away from aux *first* to avoid passing glitches when changing aux mux.
// Assume (!!!) glitchless source 0 is no faster than the aux source.
  if (has_glitchless_mux && (src == CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX)) {
    clock->ctrl &= ~ CLOCKS_CLK_REF_CTRL_SRC_BITS ;
    while ((clock->selected & 1) == 0) {}
  }else{
  // If no glitchless mux, cleanly stop the clock to avoid glitches
  // propagating when changing aux mux. Note it would be a really bad idea
  // to do this on one of the glitchless clocks (clk_sys, clk_ref).
    clock->ctrl &= ~ CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS ;
  }

//--------------------- Set aux mux first, and then glitchless mux if this clock has one
  uint32_t v = (clock->ctrl & ~CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS) ;
  v |= auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB ; // CLOCKS_CLK_REF_CTRL_AUXSRC (auxsrc) ;
  clock->ctrl = v ;

//---------------------
  if (has_glitchless_mux) {
    v = (clock->ctrl & ~CLOCKS_CLK_REF_CTRL_SRC_BITS) ;
    v |= src << CLOCKS_CLK_REF_CTRL_SRC_LSB ;
    clock->ctrl = v ;
    while ((clock->selected & (1u << src)) == 0) {}
  }

//---------------------
  clock->ctrl |= CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS ;
  clock->div = div ;
}

//--------------------------------------------------------------------------------------------------
// watchdog_start_tick
// From SDK, src/rp2_common/hardware_watchdog/watchdog.c
//--------------------------------------------------------------------------------------------------

static void watchdog_start_tick (const uint32_t inCycles) {
  watchdog_hw->tick = inCycles | WATCHDOG_TICK_ENABLE_BITS;
}

//--------------------------------------------------------------------------------------------------
// clock_init
// From Arduino, rp2040/hardware/rp2040/1.1.0/pico-sdk/src/rp2_common/hardware_clocks/clocks.c
//--------------------------------------------------------------------------------------------------

static void clock_init (void) {
//--------------------- Start tick in watchdog
  watchdog_start_tick (XOSC_MHZ) ;

//--------------------- Disable resus that may be enabled from previous software (page 231)
  clocks_hw->resus.ctrl = 0 ;

//--------------------- Enable the xosc
  xosc_init () ;

//--------------------- Before we touch PLLs,
//                      switch sys and ref cleanly away from their aux sources.
  clocks_hw->clk[clk_sys].ctrl &= ~ CLOCKS_CLK_SYS_CTRL_SRC_BITS ; // Page 226
  while (clocks_hw->clk[clk_sys].selected != 0x1) {} // Page 227

  clocks_hw->clk[clk_ref].ctrl &= ~ CLOCKS_CLK_REF_CTRL_SRC_BITS ; // Page 225
  while (clocks_hw->clk[clk_ref].selected != 0x1) {} // Page 226

//--------------------- Configure PLLs
//                   REF     FBDIV VCO            POSTDIV
// PLL SYS: 12 / 1 = 12MHz * 125 = 1500MHZ / 6 / 2 = 125MHz
// PLL USB: 12 / 1 = 12MHz * 40  = 480 MHz / 5 / 2 =  48MHz
//--- First reset PLL pheripherals
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

// CLK SYS = PLL SYS (125MHz) / 1 = 125MHz
  clock_configure (ClockIndex::clk_sys,
                   CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                   CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                   125 * MHZ,
                   CPU_MHZ * MHZ);

// CLK USB = PLL USB (48MHz) / 1 = 48MHz
  clock_configure (ClockIndex::clk_usb,
                   0, // No GLMUX
                   CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                   48 * MHZ,
                   48 * MHZ);

// CLK ADC = PLL USB (48MHZ) / 1 = 48MHz
  clock_configure (ClockIndex::clk_adc,
                   0, // No GLMUX
                   CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                   48 * MHZ,
                   48 * MHZ);

// CLK RTC = PLL USB (48MHz) / 1024 = 46875Hz
  clock_configure (ClockIndex::clk_rtc,
                   0, // No GLMUX
                   CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                   48 * MHZ,
                   46875) ;

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
// See in Pici SDK: src/rp2_common/pico_runtime/runtime.c
//--------------------------------------------------------------------------------------------------

void cpu0Phase3Boot (void) {
//------------------------------------ Reset peripherals
//--- Reset all peripherals to put system into a known state,
// - except for QSPI pads and the XIP IO bank, as this is fatal if running from flash
// - and the PLLs, as this is fatal if clock muxing has not been reset on this boot
  const uint32_t peripheralsNotResetted =
    RESETS_RESET_IO_QSPI_BITS |
    RESETS_RESET_PADS_QSPI_BITS |
    RESETS_RESET_PLL_USB_BITS |
    RESETS_RESET_PLL_SYS_BITS
  ;
  resets_hw->reset |= ~ peripheralsNotResetted ;
//--- Remove reset from peripherals which are clocked only by clk_sys and
// clk_ref. Other peripherals stay in reset until we've configured clocks.
 const uint32_t otherPeripherals =
    RESETS_RESET_ADC_BITS |
    RESETS_RESET_RTC_BITS |
    RESETS_RESET_SPI0_BITS |
    RESETS_RESET_SPI1_BITS |
    RESETS_RESET_UART0_BITS |
    RESETS_RESET_UART1_BITS |
    RESETS_RESET_USBCTRL_BITS
 ;
 const uint32_t reset = RESETS_RESET_BITS & ~ otherPeripherals ;
  resets_hw->reset &= ~reset ;
//--- Wait for reset done
  while ((reset & ~resets_hw->reset_done) != 0) {}
//------------------------------------ Configure clocks
  clock_init () ;
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
//------------------------------------ Reset other peripherals
  resets_hw->reset |= otherPeripherals ;
  resets_hw->reset &= ~ otherPeripherals ;
  while ((otherPeripherals & ~resets_hw->reset_done) != 0) {}
//------------------------------------ Configure VTOR Cortex-M0+ register
  extern uint32_t __vectors_start_cpu_0 ;
  scb_hw->vtor = uint32_t (& __vectors_start_cpu_0) ;

  while ((sio_hw->fifo_st & 1) != 0) {
    const uint32_t unused __attribute__((unused)) = sio_hw->fifo_rd ;
  }
//------------------------------------ Empty the FIFO
  while ((sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) != 0) {
    const uint32_t unused __attribute__((unused)) = sio_hw->fifo_rd ;
  }
}

//--------------------------------------------------------------------------------------------------
//   PHASE 3 INIT ROUTINE, CPU 0
//--------------------------------------------------------------------------------------------------

void cpu0Phase3Init (void) { // Executed by CPU 0
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
//--------------- Copy .run.in.ram sections in RAM
  extern uint32_t __run_in_ram_start ;
  extern const uint32_t __run_in_ram_end ;
  extern uint32_t __run_in_ram_load_start ;
  uint32_t * pSrc = & __run_in_ram_load_start ;
  uint32_t * pDest = & __run_in_ram_start ;
  while (pDest != & __run_in_ram_end) {
    * pDest = * pSrc ;
    pDest ++ ;
    pSrc ++ ;
  }
//--------------- Copy boot2 function in RAM
  extern uint32_t __flash_enable_xip_via_boot2_in_ram_base_address [64] ;
  extern uint32_t __boot2_in_flash_start [64] ;
  for (uint32_t i=0 ; i<64 ; i++) {
    __flash_enable_xip_via_boot2_in_ram_base_address [i] = __boot2_in_flash_start [i] ;
  }
}

//--------------------------------------------------------------------------------------------------
//   PHASE 3 INIT ROUTINE, CPU 1
//--------------------------------------------------------------------------------------------------

void cpu1Phase3Init (void) { // Executed by CPU 1
//--- Aller exécuter les routines d'initialisation de la section phase.3.cpu.1.init.routine.array
  extern void (* __phase_3_cpu_1_init_routine_array_start) (void) ;
  extern void (* __phase_3_cpu_1_init_routine_array_end) (void) ;
  void (* * ptr) (void) = & __phase_3_cpu_1_init_routine_array_start ;
  while (ptr != & __phase_3_cpu_1_init_routine_array_end) {
    (* ptr) () ;
    ptr ++ ;
  }
}

//--------------------------------------------------------------------------------------------------
