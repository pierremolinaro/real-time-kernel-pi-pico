#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void section_functionInRam (SECTION_MODE) {
}

//--------------------------------------------------------------------------------------------------

static void displayTask (USER_MODE) {
  functionInRam (MODE) ;
  gotoXY (MODE_ 0, 0) ;
  printHex8 (MODE_ uint32_t (functionInRam)) ;
  gotoXY (MODE_ 10, 0) ;
  printHex8 (MODE_ uint32_t (section_functionInRam)) ;
}

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [128] ;

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), displayTask) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

