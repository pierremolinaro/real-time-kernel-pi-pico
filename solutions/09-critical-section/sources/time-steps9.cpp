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

static void activateSystickInterrupt (INIT_MODE) {
  systick_hw->csr |= M0PLUS_SYST_CSR_TICKINT_BITS ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (activateSystickInterrupt) ;

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
//   SYSTICK interrupt service routine
//--------------------------------------------------------------------------------------------------

static volatile uint32_t gUptime ;

//--------------------------------------------------------------------------------------------------

void systickInterruptServiceRoutine (SECTION_MODE) {
  const uint32_t newUptime = gUptime + 1 ;
  gUptime = newUptime ;
//--- Run real.time.interrupt.routine.array section routines
  extern void (* __real_time_interrupt_routine_array_start) (SECTION_MODE_ const uint32_t inUptime) ;
  extern void (* __real_time_interrupt_routine_array_end) (SECTION_MODE_ const uint32_t inUptime) ;
  void (* * ptr) (SECTION_MODE_ const uint32_t) = & __real_time_interrupt_routine_array_start ;
  while (ptr != & __real_time_interrupt_routine_array_end) {
    (* ptr) (MODE_ newUptime) ;
    ptr ++ ;
  }
}

//--------------------------------------------------------------------------------------------------
//   millis — ANY MODE
//--------------------------------------------------------------------------------------------------

uint32_t millis (ANY_MODE) {
  return gUptime ;
}

//--------------------------------------------------------------------------------------------------
//   MICRO SECONDS
//--------------------------------------------------------------------------------------------------

uint64_t section_microseconds (SECTION_MODE) {
  const uint32_t lowValue = timer_hw->timelr ;
  const uint32_t highValue = timer_hw->timehr ;
  const uint64_t value = (uint64_t (highValue) << 32) | lowValue ;
  return value ;
}

//--------------------------------------------------------------------------------------------------
//   busyWaitDuring, busyWaitUntil — USER MODE
//--------------------------------------------------------------------------------------------------

void busyWaitDuring (USER_MODE_ const uint32_t inDelayMS) {
  busyWaitUntil (MODE_ gUptime + inDelayMS) ;
}

//--------------------------------------------------------------------------------------------------

void busyWaitUntil (USER_MODE_ const uint32_t inDeadlineMS) {
  while (inDeadlineMS > gUptime) {}
}

//--------------------------------------------------------------------------------------------------
