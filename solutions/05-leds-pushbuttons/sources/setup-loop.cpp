#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void setup0 (USER_MODE) {
}

//--------------------------------------------------------------------------------------------------

void loop0 (USER_MODE) {
//--- Complement BUILTIN_LED output
  digitalToggle (BUILTIN_LED) ;
//--- Wait...
  busyWaitDuring (MODE_ 250) ;
//---
  digitalWrite (L4_LED, !digitalRead (P4_PUSH_BUTTON)) ;
}

//--------------------------------------------------------------------------------------------------
