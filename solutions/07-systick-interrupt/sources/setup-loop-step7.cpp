#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void setup0 (USER_MODE) {
  printUnsigned (MODE_ millis (MODE)) ;
  digitalToggle (BUILTIN_LED) ;
}

//--------------------------------------------------------------------------------------------------

static uint32_t gDisplayTime = 0 ;

void loop0 (USER_MODE) {
  if (gDisplayTime <= millis (MODE)) {
    digitalToggle (L4_LED) ;
    gotoXY (MODE_ 0, 1) ;
    printUnsigned (MODE_ millis (MODE)) ;
    gDisplayTime += 1000 ;
  }
}

//--------------------------------------------------------------------------------------------------
