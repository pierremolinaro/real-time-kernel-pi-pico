#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void setup0 (USER_MODE) {
//--- Programmer l'interruption sur front descendant sur le poussoir P4
//  gpioAcknowledgeInterrupt (P4_PUSH_BUTTON, GPIO_ALL_IRQ) ;
  gpioEnableInterrupt (P4_PUSH_BUTTON, GPIO_IRQ_EDGE_FALL, true) ;
  NVIC_ENABLE_IRQ (ISRSlot::IO_IRQ_BANK0) ;
//--- Message d'accueil
  printString (MODE_ "Hello") ;
}

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gClicCount ;

#define NVIC_ICPR *((volatile uint32_t *) (0xe0000000 + 0xe280))

//--------------------------------------------------------------------------------------------------

void loop0 (USER_MODE) {
  digitalWrite (L4_LED, !digitalRead (P4_PUSH_BUTTON)) ;
  busyWaitDuring (MODE_ 1000) ;
  gotoXY (MODE_ 0, 0) ;
  printUnsigned (MODE_ millis (MODE)) ;
  gotoXY (MODE_ 0, 1) ;
  printUnsigned (MODE_ gClicCount) ;
//   gotoXY (MODE_ 0, 2) ;
//   printHex8 (MODE_ iobank0_hw->io [13].status) ;
//   gotoXY (MODE_ 0, 3) ;
//   printHex8 (MODE_ NVIC_ICPR) ;
}

//--------------------------------------------------------------------------------------------------

void clicInterrupt (SECTION_MODE) {
//--- Acquitter l'interruption
  gpioAcknowledgeInterrupt (P4_PUSH_BUTTON, GPIO_IRQ_EDGE_FALL) ;
//---
  gClicCount += 1 ;

}

//--------------------------------------------------------------------------------------------------
