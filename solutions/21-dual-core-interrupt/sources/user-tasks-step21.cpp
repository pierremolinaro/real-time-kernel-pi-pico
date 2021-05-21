#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gCountProc0 = 0 ;
// static volatile uint32_t gCountProc0PerSecond = 0 ;
static volatile uint32_t gCountProc1 = 0 ;
static volatile uint32_t gCount      = 0 ;

//--------------------------------------------------------------------------------------------------

void pwmInterruptServiceRoutine (SECTION_MODE) {
  if (digitalRead (P4_PUSH_BUTTON)) {
    if (getCoreIndex () == 0) {
      gCountProc0 += 1 ;
//       gCountProc0PerSecond += 1 ;
    }else{
      gCountProc1 += 1 ;
    }
    gCount += 1 ;
  }
//--- Acquitter l'interruption
  pwm_hw->intr = 0x01 ;
}

//--------------------------------------------------------------------------------------------------

static void displayTask (USER_MODE) {
  uint32_t deadline = 0 ;
  while (1) {
    waitUntil (MODE_ deadline) ;
    deadline += 1000 ;
//     const uint32_t countPerSecond = gCountProc0PerSecond ;
//     gCountProc0PerSecond = 0 ;
    gotoXY (MODE_ 0, 0) ;
    printUnsigned (MODE_ gCountProc0) ;
//     gotoXY (MODE_ 10, 0) ;
//     printSpaces (MODE_ 10) ;
//     gotoXY (MODE_ 10, 0) ;
//     printUnsigned (MODE_ countPerSecond) ;
    gotoXY (MODE_ 0, 1) ;
    printUnsigned (MODE_ gCountProc1) ;
    gotoXY (MODE_ 0, 2) ;
    printUnsigned (MODE_ gCount) ;
  }
}

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [64] ;

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), displayTask) ;
//--- Configure PWN channel 0
  pwm_hw->slice [0].ctr = (1 << 4) | 0  ; // DIV_INT, DIV_FRAC
  pwm_hw->slice [0].top = 12499  ; // TOP
  pwm_hw->slice [0].csr = 1 ; // PH_CORRECT = 0, DIVMODE = 0, EN = 1
  pwm_hw->inte = 0x01 ;
  NVIC_ENABLE_IRQ (ISRSlot::PWM_IRQ_WRAP) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------
//   Init CPU 1
//--------------------------------------------------------------------------------------------------

static void cpu1Init (INIT_CPU1_MODE) {
  NVIC_ENABLE_IRQ (ISRSlot::PWM_IRQ_WRAP) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_CPU1_ROUTINE (cpu1Init) ;

//--------------------------------------------------------------------------------------------------

