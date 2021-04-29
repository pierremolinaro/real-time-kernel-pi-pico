#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void setup0 (USER_MODE) {
  printUnsigned (MODE_ millis (MODE)) ;
  gotoXY (MODE_ 0, 3) ;
  printHex8 (MODE_ scb_hw->vtor) ;
}

//--------------------------------------------------------------------------------------------------

static uint32_t gDisplayTime = 0 ;

void loop0 (USER_MODE) {
  if (gDisplayTime <= millis (MODE)) {
    const uint32_t s = systick_current_cpu (MODE) ;
    digitalToggle (L4_LED) ;
    gotoXY (MODE_ 0, 1) ;
    printUnsigned (MODE_ s) ;
    gotoXY (MODE_ 0, 2) ;
    printUnsigned (MODE_ millis (MODE)) ;
    gDisplayTime += 1000 ;
  }
}

//--------------------------------------------------------------------------------------------------
