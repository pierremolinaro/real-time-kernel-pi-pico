#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

inline static void __sev (void) {
  __asm volatile ("sev") ;
}

//--------------------------------------------------------------------------------------------------

static inline void multicore_fifo_drain (void) {
  while ((sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) != 0) {
    const uint32_t unused __attribute__((unused)) = sio_hw->fifo_rd ;
  }
}

//--------------------------------------------------------------------------------------------------

static inline void multicore_fifo_push_blocking (const uint32_t inData) {
//--- We wait for the fifo to have some space
   // return !!(sio_hw->fifo_st & SIO_FIFO_ST_RDY_BITS);
  while ((sio_hw->fifo_st & SIO_FIFO_ST_RDY_BITS) == 0) { }
//  while (!multicore_fifo_wready ()) { }
//--- Send data
  sio_hw->fifo_wr = inData ;  // Page 50
//  sio_hw->fifo_wr = data;
//--- Fire off an event to the other core
  __sev () ;
}

//--------------------------------------------------------------------------------------------------

static uint32_t multicore_fifo_pop_blocking (void) {
// If nothing there yet, we wait for an event first,
// to try and avoid too much busy waiting
  while ((sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) == 0) {
    __asm volatile ("wfe") ;
  }
//  while (!multicore_fifo_rvalid()) { __wfe(); }
//  return sio_hw->fifo_rd;
  return sio_hw->fifo_rd ;
}

//--------------------------------------------------------------------------------------------------

static void multicore_launch_core1_raw (void (*cpu1Code)(void),
                                        const uint32_t inTopOfStackCPU1,
                                        const uint32_t inCPU1VectorTable) {
  const uint32_t cmd_sequence [6] = {
    0,
    0,
    1,
    inCPU1VectorTable,
    inTopOfStackCPU1,
    uint32_t (cpu1Code)
  } ;
  uint32_t seq = 0 ;
  do {
    const uint32_t cmd = cmd_sequence [seq] ;
  //--- we drain before sending a 0
    if (cmd == 0) {
      multicore_fifo_drain ();
      __sev () ; // core 1 may be waiting for fifo space
    }
    multicore_fifo_push_blocking (cmd) ;
    const uint32_t response = multicore_fifo_pop_blocking () ;
  //--- move to next state on correct response otherwise start over
    seq = (cmd == response) ? seq + 1 : 0 ;
  } while (seq < 6) ;
}

//--------------------------------------------------------------------------------------------------

static void cpu1_code (void) {
  while (1) {
  //--- Drive BUILTIN_LED high --> led is on
    digitalWrite (BUILTIN_LED, true) ;
  //--- Wait...
    for (volatile uint32_t i=0 ; i< 100 * 1000 ; i++) {}
  //--- Drive BUILTIN_LED low --> led is off
    digitalWrite (BUILTIN_LED, false) ;
  //--- Wait...
    for (volatile uint32_t i=0 ; i< 500 * 1000 ; i++) {}
  }
}

//--------------------------------------------------------------------------------------------------

static void start_cpu_1 (INIT_MODE) { // Code run by CPU 0
  extern uint32_t __vectors_start_cpu_0 ;
  const uint32_t vectors_cpu_0 = uint32_t (& __vectors_start_cpu_0) ;
  extern uint32_t __system_stack_end_cpu_1 ;
  const uint32_t top_of_system_stack_cpu_1 = uint32_t (& __system_stack_end_cpu_1) ;
  multicore_launch_core1_raw (cpu1_code, top_of_system_stack_cpu_1, vectors_cpu_0) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (start_cpu_1) ;

//--------------------------------------------------------------------------------------------------