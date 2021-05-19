#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

volatile uint32_t gCpu1AllocationCount = 0 ;

//--------------------------------------------------------------------------------------------------

void cpu1_code (void) asm ("reset.handler.cpu.1") ;

void cpu1_code (void) {
  while (1) {
     digitalWrite (DigitalPort::GP27, true) ;
     const uint8_t * buffer = new uint8_t [50] ;
     delete [] buffer ;
     gCpu1AllocationCount += 1 ;
  }
}

//--------------------------------------------------------------------------------------------------
