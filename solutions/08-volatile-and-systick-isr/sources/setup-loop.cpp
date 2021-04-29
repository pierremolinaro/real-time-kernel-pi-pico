#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gCount0 = 0 ;
static volatile uint32_t gCount1 = 0 ;
static volatile uint32_t gCount2 = 0 ;
static volatile uint32_t gCount3 = 0 ;
static volatile bool gDisplay = true ;

//--------------------------------------------------------------------------------------------------

static void rtISR (SECTION_MODE_ const uint32_t inUptime) {
  if (gDisplay) {
    gCount0 += 1 ;
    gCount1 += 1 ;
    gCount2 += 1 ;
    gCount3 += 1 ;
  }
}

//--------------------------------------------------------------------------------------------------

MACRO_REAL_TIME_ISR (rtISR) ;

//--------------------------------------------------------------------------------------------------

void setup (USER_MODE) {
}

//--------------------------------------------------------------------------------------------------

void loop (USER_MODE) {
  if (millis (MODE) < 5000) {
    gCount0 += 1 ;
    gCount1 += 1 ;
    gCount2 += 1 ;
    gCount3 += 1 ;
  }else if (gDisplay) {
    gotoXY (MODE_ 0, 0) ;
    printUnsigned (MODE_ gCount0) ;
    gotoXY (MODE_ 0, 1) ;
    printUnsigned (MODE_ gCount1) ;
    gotoXY (MODE_ 0, 2) ;
    printUnsigned (MODE_ gCount2) ;
    gotoXY (MODE_ 0, 3) ;
    printUnsigned (MODE_ gCount3) ;
    gDisplay = false ;
  }
}

//--------------------------------------------------------------------------------------------------
