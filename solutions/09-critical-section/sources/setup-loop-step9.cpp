#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gCount0 = 0 ;
static volatile uint32_t gCount1 = 0 ;
static volatile uint32_t gCount2 = 0 ;
static volatile uint32_t gCount3 = 0 ;
static volatile bool gPerformCount = true ;

//--------------------------------------------------------------------------------------------------

static void rtISR (SECTION_MODE_ const uint32_t inUptime) {
  if (gPerformCount) {
    gCount0 += 1 ;
    gCount1 += 1 ;
    gCount2 += 1 ;
    gCount3 += 1 ;
  }
}

//--------------------------------------------------------------------------------------------------

MACRO_REAL_TIME_ISR (rtISR) ;

//--------------------------------------------------------------------------------------------------

void setup0 (USER_MODE) {
  printString (MODE_ "Wait...") ;
}

//--------------------------------------------------------------------------------------------------

void section_incrementations (SECTION_MODE) {
  gCount0 += 1 ;
  gCount1 += 1 ;
  gCount2 += 1 ;
  gCount3 += 1 ;
}

//--------------------------------------------------------------------------------------------------

void loop0 (USER_MODE) {
  if (gPerformCount) {
    incrementations (MODE) ;
    if (millis (MODE) >= 5000) {
      gPerformCount = false ;
      gotoXY (MODE_ 0, 0) ;
      printUnsigned (MODE_ gCount0) ;
      gotoXY (MODE_ 0, 1) ;
      printUnsigned (MODE_ gCount1) ;
      gotoXY (MODE_ 0, 2) ;
      printUnsigned (MODE_ gCount2) ;
      gotoXY (MODE_ 0, 3) ;
      printUnsigned (MODE_ gCount3) ;
    }
  }else{
    busyWaitDuring (MODE_ 1000) ;
    gotoXY (MODE_ 10, 0) ;
    printUnsigned (MODE_ millis (MODE)) ;
    gotoXY (MODE_ 10, 1) ;
    printUnsigned64 (MODE_ microseconds (MODE)) ;
  }
}

//--------------------------------------------------------------------------------------------------
