#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [64] ;
static uint64_t gStack1 [64] ;
static uint64_t gStack2 [64] ;
static uint64_t gStack3 [64] ;
static uint64_t gStack4 [64] ;

//--------------------------------------------------------------------------------------------------

static Semaphore task0Completed (0) ;
static Semaphore task1Completed (0) ;
static Semaphore task2Completed (0) ;
static Semaphore task3Completed (0) ;
static Semaphore mutexLCD (1) ;

//--------------------------------------------------------------------------------------------------

static void task0 (USER_MODE) {
  for (uint32_t i=0 ; i < 10 ; i++) {
    digitalToggle (L0_LED) ;
    waitDuring (MODE_ 250) ;
  }
  task0Completed.V (MODE) ;
  task0Completed.V (MODE) ;
  mutexLCD.P (MODE) ;
    gotoXY (MODE_ 0, 0) ;
    printString (MODE_ "TASK 0") ;
  mutexLCD.V (MODE) ;
}

//--------------------------------------------------------------------------------------------------

static void task1 (USER_MODE) {
  task0Completed.P (MODE) ;
  for (uint32_t i=0 ; i < 10 ; i++) {
    digitalToggle (L1_LED) ;
    waitDuring (MODE_ 261) ;
  }
  task1Completed.V (MODE) ;
  mutexLCD.P (MODE) ;
    gotoXY (MODE_ 10, 0) ;
    printString (MODE_ "TASK 1") ;
  mutexLCD.V (MODE) ;
}

//--------------------------------------------------------------------------------------------------

static void task2 (USER_MODE) {
  task0Completed.P (MODE) ;
  for (uint32_t i=0 ; i < 10 ; i++) {
    digitalToggle (L2_LED) ;
    waitDuring (MODE_ 272) ;
  }
  task2Completed.V (MODE) ;
  mutexLCD.P (MODE) ;
    gotoXY (MODE_ 0, 1) ;
    printString (MODE_ "TASK 2") ;
  mutexLCD.V (MODE) ;
}

//--------------------------------------------------------------------------------------------------

static void task3 (USER_MODE) {
  task1Completed.P (MODE) ;
  for (uint32_t i=0 ; i < 10 ; i++) {
    digitalToggle (L3_LED) ;
    waitDuring (MODE_ 283) ;
  }
  task3Completed.V (MODE) ;
  mutexLCD.P (MODE) ;
    gotoXY (MODE_ 10, 1) ;
    printString (MODE_ "TASK 3") ;
  mutexLCD.V (MODE) ;
}

//--------------------------------------------------------------------------------------------------

static void task4 (USER_MODE) {
  task3Completed.P (MODE) ;
  task2Completed.P (MODE) ;
  for (uint32_t i=0 ; i < 10 ; i++) {
    digitalToggle (L4_LED) ;
    waitDuring (MODE_ 294) ;
  }
  mutexLCD.P (MODE) ;
    gotoXY (MODE_ 0, 2) ;
    printString (MODE_ "TASK 4") ;
  mutexLCD.V (MODE) ;
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

