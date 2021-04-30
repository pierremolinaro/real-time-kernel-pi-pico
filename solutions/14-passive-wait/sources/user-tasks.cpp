#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [64] ;
static uint64_t gStack1 [64] ;
static uint64_t gStack2 [64] ;
static uint64_t gStack3 [64] ;
static uint64_t gStack4 [64] ;

//--------------------------------------------------------------------------------------------------

static void task0 (USER_MODE) {
  for (uint32_t i=0 ; i < 100 ; i++) {
    digitalToggle (L0_LED) ;
    waitDuring (MODE_ 250) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void task1 (USER_MODE) {
  for (uint32_t i=0 ; i < 100 ; i++) {
    digitalToggle (L1_LED) ;
    waitDuring (MODE_ 261) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void task2 (USER_MODE) {
  for (uint32_t i=0 ; i < 100 ; i++) {
    digitalToggle (L2_LED) ;
    waitDuring (MODE_ 272) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void task3 (USER_MODE) {
  for (uint32_t i=0 ; i < 100 ; i++) {
    digitalToggle (L3_LED) ;
    waitDuring (MODE_ 283) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void task4 (USER_MODE) {
  for (uint32_t i=0 ; i < 100 ; i++) {
    digitalToggle (L4_LED) ;
    waitDuring (MODE_ 294) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), task0) ;
  kernel_createTask (MODE_ gStack1, sizeof (gStack1), task1) ;
  kernel_createTask (MODE_ gStack2, sizeof (gStack2), task2) ;
  kernel_createTask (MODE_ gStack3, sizeof (gStack3), task3) ;
  kernel_createTask (MODE_ gStack4, sizeof (gStack4), task4) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

