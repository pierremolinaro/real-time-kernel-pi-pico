#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

volatile uint32_t gCpu1AllocationCount = 0 ;
volatile uint32_t gCpu1Parameter = 0 ;

//--------------------------------------------------------------------------------------------------

void cpu1_code (void) asm ("reset.handler.cpu.1") ;

void cpu1_code (void) {
  asm volatile ("isb") ;
  asm volatile ("dsb") ;
  asm volatile ("dmb") ;
  while (1) {
     digitalWrite (DigitalPort::GP27, true) ;
     asm volatile ("mov %[result], sp" : [result] "=r" (gCpu1Parameter));
//     const uint8_t * buffer = new uint8_t [50] ;
//     delete [] buffer ;
//     gCpu1AllocationCount += 1 ;
  }
}

//--------------------------------------------------------------------------------------------------
