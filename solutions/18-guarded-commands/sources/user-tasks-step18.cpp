#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static Semaphore s0 (0) ;
static Semaphore s1 (0) ;
static Semaphore s2 (0) ;
static Semaphore s3 (0) ;
static Semaphore s4 (0) ;

//--------------------------------------------------------------------------------------------------

static void task0 (USER_MODE) {
  while (1) {
    waitDuring (MODE_ 200) ;
    s0.V (MODE) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void task1 (USER_MODE) {
  while (1) {
    waitDuring (MODE_ 250) ;
    s1.V (MODE) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void task2 (USER_MODE) {
  while (1) {
    waitDuring (MODE_ 300) ;
    s2.V (MODE) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void task3 (USER_MODE) {
  while (1) {
    waitDuring (MODE_ 350) ;
    s3.V (MODE) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void task4 (USER_MODE) {
  while (1) {
    waitDuring (MODE_ 400) ;
    s4.V (MODE) ;
  }
}

//--------------------------------------------------------------------------------------------------

static void blinkTask (USER_MODE) {
  while (1) {
    if (s0.guarded_P (MODE)) {
      digitalToggle (L0_LED) ;
    }else if (s1.guarded_P (MODE)) {
      digitalToggle (L1_LED) ;
    }else if (s2.guarded_P (MODE)) {
      digitalToggle (L2_LED) ;
    }else if (s3.guarded_P (MODE)) {
      digitalToggle (L3_LED) ;
    }else if (s4.guarded_P (MODE)) {
      digitalToggle (L4_LED) ;
    }else{
      guard_waitForChange (MODE) ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [64] ;
static uint64_t gStack1 [64] ;
static uint64_t gStack2 [64] ;
static uint64_t gStack3 [64] ;
static uint64_t gStack4 [64] ;
static uint64_t gStackBlink [64] ;

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), task0) ;
  kernel_createTask (MODE_ gStack1, sizeof (gStack1), task1) ;
  kernel_createTask (MODE_ gStack2, sizeof (gStack2), task2) ;
  kernel_createTask (MODE_ gStack3, sizeof (gStack3), task3) ;
  kernel_createTask (MODE_ gStack4, sizeof (gStack4), task4) ;
  kernel_createTask (MODE_ gStackBlink, sizeof (gStackBlink), blinkTask) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

