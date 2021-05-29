#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gCountTask0 = 0 ;
static volatile uint32_t gCountPerSecondTask0 = 0 ;
static volatile bool gResetCountPerSecondTask0 = false ;
static volatile uint32_t gCountTask1 = 0 ;
static volatile uint32_t gCountPerSecondTask1 = 0 ;
static volatile bool gResetCountPerSecondTask1 = false ;

//--------------------------------------------------------------------------------------------------

static void task0 (USER_MODE) {
  while (1) {
    if (gResetCountPerSecondTask0) {
      gResetCountPerSecondTask0 = false ;
      gCountPerSecondTask0 = 0 ;
    }
    if (digitalRead (P4_PUSH_BUTTON)) {
      gCountTask0 += 1 ;
      gCountPerSecondTask0 += 1 ;
    }else{
      waitDuring (MODE_ 10) ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

static void task1 (USER_MODE) {
  while (1) {
    if (gResetCountPerSecondTask1) {
      gResetCountPerSecondTask1 = false ;
      gCountPerSecondTask1 = 0 ;
    }
    if (digitalRead (P3_PUSH_BUTTON)) {
      gCountTask1 += 1 ;
      gCountPerSecondTask1 += 1 ;
    }else{
      waitDuring (MODE_ 10) ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

static void displayTask (USER_MODE) {
  while (1) {
    waitDuring (MODE_ 1000) ;
    gotoXY (MODE_ 0, 0) ;
    printUnsigned (MODE_ gCountTask0) ;
    gotoXY (MODE_ 11, 0) ;
    printSpaces (MODE_ 9) ;
    gotoXY (MODE_ 11, 0) ;
    printUnsigned (MODE_ gCountPerSecondTask0) ;
    gResetCountPerSecondTask0 = true ;
    gotoXY (MODE_ 0, 1) ;
    printUnsigned (MODE_ gCountTask1) ;
    gotoXY (MODE_ 11, 1) ;
    printSpaces (MODE_ 9) ;
    gotoXY (MODE_ 11, 1) ;
    printUnsigned (MODE_ gCountPerSecondTask1) ;
    gResetCountPerSecondTask1 = true ;
  }
}

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [64] ;
static uint64_t gStack1 [64] ;
static uint64_t gStack2 [64] ;

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), displayTask) ;
  kernel_createTask (MODE_ gStack1, sizeof (gStack1), task0) ;
  kernel_createTask (MODE_ gStack2, sizeof (gStack2), task1) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

