#pragma once

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include <stdint.h>

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

enum class DigitalPort {
//--- Common with Teensy 3.1 / 3.2
    D0,  // PTB16
    D1,  // PTB17
    D2,  // PTD0
    D3,  // PTA12
    D4,  // PTA13
    D5,  // PTD7
    D6,  // PTD4
    D7,  // PTD2
    D8,  // PTD3
    D9,  // PTC3
    D10, // PTC4
    D11, // PTC6
    D12, // PTC7
    D13, // PTC5
    D14, // PTD1
    D15, // PTC0
    D16, // PTB0
    D17, // PTB1
    D18, // PTB3
    D19, // PTB2
    D20, // PTD5
    D21, // PTD6
    D22, // PTC1
    D23, // PTC2
//--- Only on Teensy 3.6
    D24, // PTE26
    D25, // PTA5
    D26, // PTA14
    D27, // PTA15
    D28, // PTA16
    D29, // PTB18
    D30, // PTB19
    D31, // PTB10
    D32, // PTB11
    D33, // PTE24
    D34, // PTE25
    D35, // PTC8
    D36, // PTC9
    D37, // PTC10
    D38, // PTC11
    D39, // PTA17
    D40, // PTA28
    D41, // PTA29
    D42, // PTA26
    D43, // PTB20
    D44, // PTB22
    D45, // PTB23
    D46, // PTB21
    D47, // PTD8
    D48, // PTD9
    D49, // PTB4
    D50, // PTB5
    D51, // PTD14
    D52, // PTD13
    D53, // PTD12
    D54, // PTD15
    D55, // PTD11
    D56, // PTE10
    D57, // PTE11
//--- No port
    True, // Fictive port: no effect on write, always read true
    False // Fictive port: no effect on write, always read false
} ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

enum class DigitalMode {
    OUTPUT,
    OUTPUT_OPEN_COLLECTOR,
    INPUT,
    INPUT_PULLDOWN,
    INPUT_PULLUP
} ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

STATIC void pinMode (const DigitalPort inPort, const DigitalMode inMode) ;

STATIC bool digitalRead (const DigitalPort inPort) ;

STATIC void digitalWrite (const DigitalPort inPort, const bool inValue) ;

STATIC void digitalToggle (const DigitalPort inPort) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
