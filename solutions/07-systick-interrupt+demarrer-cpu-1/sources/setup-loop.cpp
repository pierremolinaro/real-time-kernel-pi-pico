#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void setup (USER_MODE) {
  printUnsigned (MODE_ millis (MODE)) ;
  gotoXY (MODE_ 0, 3) ;
  printHex8 (MODE_ scb_hw->vtor) ;
}

//--------------------------------------------------------------------------------------------------

static uint32_t gDisplayTime = 0 ;

void loop (USER_MODE) {
  if (gDisplayTime <= millis (MODE)) {
    const uint32_t s = systick_cpu_0 (MODE) ;
    digitalToggle (L4_LED) ;
    gotoXY (MODE_ 0, 1) ;
    printUnsigned (MODE_ s) ;
    gotoXY (MODE_ 0, 2) ;
    printUnsigned (MODE_ millis (MODE)) ;
    gDisplayTime += 1000 ;
  }
}

//--------------------------------------------------------------------------------------------------
