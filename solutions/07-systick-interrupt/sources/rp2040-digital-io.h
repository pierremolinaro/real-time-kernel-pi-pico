#pragma once

//--------------------------------------------------------------------------------------------------

#include <stdint.h>

//--------------------------------------------------------------------------------------------------

enum class DigitalPort {
  GP0,   GP1, GP2,   GP3,  GP4,  GP5,  GP6,  GP7,  GP8,  GP9,
  GP10, GP11, GP12, GP13, GP14, GP15, GP16, GP17, GP18, GP19,
  GP20, GP21, GP22, GP23, GP24, GP25, GP26, GP27, GP28, GP29,
//--- No port
  True, // Fictive port: no effect on write, always read true
  False // Fictive port: no effect on write, always read false
} ;

//--------------------------------------------------------------------------------------------------

enum class DigitalMode {
    OUTPUT,
    INPUT,
    INPUT_PULLDOWN,
    INPUT_PULLUP
} ;

//--------------------------------------------------------------------------------------------------

void pinMode (const DigitalPort inPort, const DigitalMode inMode) ;

bool digitalRead (const DigitalPort inPort) ;

void digitalWrite (const DigitalPort inPort, const bool inValue) ;

void digitalToggle (const DigitalPort inPort) ;

//--------------------------------------------------------------------------------------------------
