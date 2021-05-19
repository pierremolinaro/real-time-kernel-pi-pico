#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void cpu1_code (void) asm ("reset.handler.cpu.1") ;

void cpu1_code (void) {
  NVIC_ENABLE_IRQ (ISRSlot::PWM_IRQ_WRAP) ;
  while (1) {
  //--- Drive GP27 high --> led is on
    digitalWrite (DigitalPort::GP27, true) ;
  //--- Wait...
    for (volatile uint32_t i=0 ; i< 100 * 1000 ; i++) {}
  //--- Drive GP27 low --> led is off
    digitalWrite (DigitalPort::GP27, false) ;
  //--- Wait...
    for (volatile uint32_t i=0 ; i< 500 * 1000 ; i++) {}
  }
}

//--------------------------------------------------------------------------------------------------
