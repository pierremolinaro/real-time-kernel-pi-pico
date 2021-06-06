#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

extern uint16_t bootromFunctionAddressArray [14] asm ("bootrom.function.address.array") ;

//--------------------------------------------------------------------------------------------------

static void displayTask (USER_MODE) {
  gotoXY (MODE_ 0, 0) ;
  printHex4 (MODE_ bootromFunctionAddressArray [0]) ;
  gotoXY (MODE_ 5, 0) ;
  printHex4 (MODE_ bootromFunctionAddressArray [1]) ;
  gotoXY (MODE_ 10, 0) ;
  printHex4 (MODE_ bootromFunctionAddressArray [2]) ;
  gotoXY (MODE_ 15, 0) ;
  printHex4 (MODE_ bootromFunctionAddressArray [3]) ;

  gotoXY (MODE_ 0, 1) ;
  printHex4 (MODE_ bootromFunctionAddressArray [4]) ;
  gotoXY (MODE_ 5, 1) ;
  printHex4 (MODE_ bootromFunctionAddressArray [5]) ;
  gotoXY (MODE_ 10, 1) ;
  printHex4 (MODE_ bootromFunctionAddressArray [6]) ;
  gotoXY (MODE_ 15, 1) ;
  printHex4 (MODE_ bootromFunctionAddressArray [7]) ;

  gotoXY (MODE_ 0, 2) ;
  printHex4 (MODE_ bootromFunctionAddressArray [8]) ;
  gotoXY (MODE_ 5, 2) ;
  printHex4 (MODE_ bootromFunctionAddressArray [9]) ;
  gotoXY (MODE_ 10, 2) ;
  printHex4 (MODE_ bootromFunctionAddressArray [10]) ;
  gotoXY (MODE_ 15, 2) ;
  printHex4 (MODE_ bootromFunctionAddressArray [11]) ;


  gotoXY (MODE_ 0, 3) ;
  printHex4 (MODE_ bootromFunctionAddressArray [12]) ;
  gotoXY (MODE_ 5, 3) ;
  printHex4 (MODE_ bootromFunctionAddressArray [13]) ;
}

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [64] ;

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), displayTask) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

