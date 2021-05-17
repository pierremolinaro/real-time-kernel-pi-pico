#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static uint64_t gStack1 [64] ;

//--------------------------------------------------------------------------------------------------

static uint32_t gDisplayTime = 0 ;

static void task1 (USER_MODE) {
  while (1) {
    if (gDisplayTime <= millis (MODE)) {
      digitalWrite (L4_LED, !digitalRead (P4_PUSH_BUTTON)) ;
      const uint64_t s = microseconds (MODE) ;
      gotoXY (MODE_ 0, 1) ;
      printUnsigned64 (MODE_ s) ;
      gotoXY (MODE_ 0, 2) ;
      printUnsigned (MODE_ millis (MODE)) ;
      gDisplayTime += 1000 ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack1, sizeof (gStack1), task1) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

