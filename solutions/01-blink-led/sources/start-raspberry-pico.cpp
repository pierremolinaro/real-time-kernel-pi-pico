#include "all-headers.h"

//--------------------------------------------------------------------------------------------------
//  BOOT ROUTINE
//--------------------------------------------------------------------------------------------------

void startPhase1 (void) {
//---
  RESETS_RESET = ~ (RESETS_RESET_io_qspi |
            RESETS_RESET_pads_qspi |
            RESETS_RESET_pll_usb |
            RESETS_RESET_pll_sys) ;
  const uint32_t reset = RESETS_RESET & ~(
            RESETS_RESET_adc |
            RESETS_RESET_rtc|
            RESETS_RESET_spi0 |
            RESETS_RESET_spi1 |
            RESETS_RESET_uart0 |
            RESETS_RESET_uart1 |
            RESETS_RESET_usbctrl
    );
  RESETS_RESET &= ~reset ;

  while (~RESETS_RESET_DONE & reset) {}

  WATCHDOG_TICK = 0 ;

//   PSM_FRCE_ON = 0x1FFFF ;

   for (volatile uint32_t i=0 ; i< 1 * 1000 * 1000 ; i++) {}

//  while (PSM_DONE != 0x1FFFF) {}

//--- Configure GP25 as output digital port
  PADS_BANK0_GPIO25 = 0x56 ;

 // sio_hw->gpio_oe_set = (1 << 25) ;
  SIO_GPIO_OE_SET = 1 << 25 ;
//  iobank0_hw->io[25].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB ;
  IO_BANK0_GPIO25_CTRL = 5 ;

  while (1) {
  SIO_GPIO_OUT_SET = 1 << 25 ;
//--- Wait...
  for (volatile uint32_t i=0 ; i< 1 * 1000 * 1000 ; i++) {}
//--- Drive GP25 low --> led is off
//  sio_hw->gpio_clr = (1 << 25) ;
  SIO_GPIO_OUT_CLR = 1 << 25 ;
//--- Wait...
  for (volatile uint32_t i=0 ; i< 1 * 1000 * 1000 ; i++) {}
  }

//  SIO_GPIO_OUT_SET = 1 << 25 ;

//------------------------------------ Clear '.bss' section
//   extern uint32_t __bss_start ;
//   extern const uint32_t __bss_end ;
//   uint32_t * p = & __bss_start ;
//   while (p != & __bss_end) {
//     * p = 0 ;
//     p ++ ;
//   }
//------------------------------------ Copy .data section in RAM
//   extern uint32_t __data_start ;
//   extern const uint32_t __data_end ;
//   extern uint32_t __data_load_start ;
//   uint32_t * pSrc = & __data_load_start ;
//   uint32_t * pDest = & __data_start ;
//   while (pDest != & __data_end) {
//     * pDest = * pSrc ;
//     pDest ++ ;
//     pSrc ++ ;
//   }
}

//--------------------------------------------------------------------------------------------------
//   PHASE 2
//--------------------------------------------------------------------------------------------------

void startPhase2 (void) {
//------------------------------------ Aller exécuter les routines d'initialisation de la section boot.routine.array
  extern void (* __boot_routine_array_start) (void) ;
  extern void (* __boot_routine_array_end) (void) ;
  void (* * ptr) (void) = & __boot_routine_array_start ;
  while (ptr != & __boot_routine_array_end) {
    (* ptr) () ;
    ptr ++ ;
  }
//------------------------------------ Run C++ global variable constructors
  extern void (* __constructor_array_start) (void) ;
  extern void (* __constructor_array_end) (void) ;
  ptr = & __constructor_array_start ;
  while (ptr != & __constructor_array_end) {
    (* ptr) () ;
    ptr ++ ;
  }
//------------------------------------ Aller exécuter les routines d'initialisation de la section init.routine.array
  extern void (* __init_routine_array_start) (void) ;
  extern void (* __init_routine_array_end) (void) ;
  ptr = & __init_routine_array_start ;
  while (ptr != & __init_routine_array_end) {
    (* ptr) () ;
    ptr ++ ;
  }
}

//--------------------------------------------------------------------------------------------------
