#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static void taskCode (USER_MODE) {
  uint32_t deadline = 1000 ;
  while (1) {
    gotoXY (MODE_ 0, 0) ;
    const uint64_t lowValue = timer_hw->timelr ;
    const uint64_t highValue = timer_hw->timehr ;
    const uint64_t value = (highValue << 32) | lowValue ;
    const uint32_t uptime = millis (MODE) ;
    printUnsigned64 (MODE_ value) ;
    gotoXY (MODE_ 0, 1) ;
    printUnsigned (MODE_ uptime) ;
    waitUntil (MODE_ deadline) ;
    deadline += 1000 ;
  }
}

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [64] ;

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), taskCode) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

