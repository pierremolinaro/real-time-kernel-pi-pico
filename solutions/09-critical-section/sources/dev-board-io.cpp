#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static void configurePorts (INIT_MODE) {
//--- Leds
  pinMode (L0_LED, DigitalMode::OUTPUT) ;
  pinMode (L1_LED, DigitalMode::OUTPUT) ;
  pinMode (L2_LED, DigitalMode::OUTPUT) ;
  pinMode (L3_LED, DigitalMode::OUTPUT) ;
  pinMode (L4_LED, DigitalMode::OUTPUT) ;
//--- Push buttons
  pinMode (P0_PUSH_BUTTON, DigitalMode::INPUT_PULLUP) ;
  pinMode (P1_PUSH_BUTTON, DigitalMode::INPUT_PULLUP) ;
  pinMode (P2_PUSH_BUTTON, DigitalMode::INPUT_PULLUP) ;
  pinMode (P3_PUSH_BUTTON, DigitalMode::INPUT_PULLUP) ;
  pinMode (P4_PUSH_BUTTON, DigitalMode::INPUT_PULLUP) ;
//--- Raspberry Pi Pico Led
  pinMode (BUILTIN_LED, DigitalMode::OUTPUT) ;
//--- Activity Leds
  pinMode (DigitalPort::GP26, DigitalMode::OUTPUT) ; // Activity 0
  digitalWrite (DigitalPort::GP26, true) ; // On
  pinMode (DigitalPort::GP27, DigitalMode::OUTPUT) ; // Activity 1
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (configurePorts) ;

//--------------------------------------------------------------------------------------------------
