#include "all-headers.h"

//--------------------------------------------------------------------------------------------------
//   PHASE 3 INIT ROUTINE, CPU 0
//--------------------------------------------------------------------------------------------------

static void copyFunctionsInRam (INIT_MODE) { // Executed by CPU 0
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

MACRO_INIT_ROUTINE (copyFunctionsInRam) ;

//--------------------------------------------------------------------------------------------------
