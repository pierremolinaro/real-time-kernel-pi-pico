#pragma once

//--------------------------------------------------------------------------------------------------

#include <stdint.h>

//--------------------------------------------------------------------------------------------------

enum class DigitalPort {
  GP0  =  0, GP1  =  1, GP2  =  2, GP3  =  3,
  GP4  =  4, GP5  =  5, GP6  =  6, GP7  =  7,
  GP8  =  8, GP9  =  9, GP10 = 10, GP11 = 11,
  GP12 = 12, GP13 = 13, GP14 = 14, GP15 = 15,
  GP16 = 16, GP17 = 17, GP18 = 18, GP19 = 19,
  GP20 = 20, GP21 = 21, GP22 = 22, GP23 = 23,
  GP24 = 24, GP25 = 25, GP26 = 26, GP27 = 27,
  GP28 = 28, GP29 = 29
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
// INTERRUPTS
//--------------------------------------------------------------------------------------------------

static const uint32_t GPIO_IRQ_LEVEL_LOW  = 0x1u ;
static const uint32_t GPIO_IRQ_LEVEL_HIGH = 0x2u ;
static const uint32_t GPIO_IRQ_EDGE_FALL  = 0x4u ;
static const uint32_t GPIO_IRQ_EDGE_RISE  = 0x8u ;
static const uint32_t GPIO_ALL_IRQ        = 0xFu ;

//--------------------------------------------------------------------------------------------------

void gpioEnableInterrupt (const DigitalPort inPort, const uint32_t inEvents, const bool inEnable) ;

//--------------------------------------------------------------------------------------------------

void gpioAcknowledgeInterrupt (const DigitalPort inPort, const uint32_t inEvents) ;

//--------------------------------------------------------------------------------------------------
