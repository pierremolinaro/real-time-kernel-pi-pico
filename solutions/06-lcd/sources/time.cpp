#include "all-headers.h"

//--------------------------------------------------------------------------------------------------
//   Configure systick
//--------------------------------------------------------------------------------------------------

static void startSystick (BOOT_MODE) {
//------------------------------------ Configure Systick
  systick_hw->rvr = CPU_MHZ * 1000 - 1 ; // Underflow every ms
  systick_hw->cvr = 0 ;
  systick_hw->csr = M0PLUS_SYST_CSR_CLKSOURCE_BITS | M0PLUS_SYST_CSR_ENABLE_BITS ;
}

//--------------------------------------------------------------------------------------------------

MACRO_BOOT_ROUTINE (startSystick) ;

//--------------------------------------------------------------------------------------------------
//   busyWaitDuring — INIT MODE
//--------------------------------------------------------------------------------------------------

void busyWaitDuring_initMode (INIT_MODE_ const uint32_t inDelayMS) {
  for (uint32_t i=0 ; i<inDelayMS ; i++) {
 // Busy wait, polling COUNTFLAG
    while ((systick_hw->csr & M0PLUS_SYST_CSR_COUNTFLAG_BITS) == 0) {}
  }
}

//--------------------------------------------------------------------------------------------------
//   busyWaitDuring — USER MODE
//--------------------------------------------------------------------------------------------------

void busyWaitDuring (USER_MODE_ const uint32_t inDelayMS) {
  for (uint32_t i=0 ; i<inDelayMS ; i++) {
 // Busy wait, polling COUNTFLAG
    while ((systick_hw->csr & M0PLUS_SYST_CSR_COUNTFLAG_BITS) == 0) {}
  }
}

//--------------------------------------------------------------------------------------------------
