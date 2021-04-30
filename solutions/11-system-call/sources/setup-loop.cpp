#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void setup0 (USER_MODE) {
  printString (MODE_ "Hello!") ;
}

//--------------------------------------------------------------------------------------------------

static uint32_t gDisplayTime = 0 ;

void loop0 (USER_MODE) {
  digitalWrite (L4_LED, !digitalRead (P4_PUSH_BUTTON)) ;
  if (gDisplayTime <= millis (MODE)) {
//     const uint32_t s = systick (MODE) ;
//     gotoXY (MODE_ 0, 1) ;
//     printUnsigned (MODE_ s) ;
    gotoXY (MODE_ 0, 2) ;
    printUnsigned (MODE_ millis (MODE)) ;
//     gotoXY (MODE_ 0, 3) ;
//     printUnsigned64 (MODE_ micros (MODE)) ;
    gDisplayTime += 1000 ;
  }
}

//--------------------------------------------------------------------------------------------------
