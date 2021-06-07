//--------------------------------------------------------------------------------------------------
//   INTERRUPT VECTORS
//--------------------------------------------------------------------------------------------------

  .syntax unified
  .cpu cortex-m0plus
  .thumb

//--------------------------------------------------------------------------------------------------
// CPU 0
//--------------------------------------------------------------------------------------------------

  .section isr.vectors.cpu.0, "a", %progbits

//--------------------------------------------------------------------------------------------------

  .word __system_stack_end_cpu_0
//--- ARM Core System Handler Vectors
  .word reset.handler.cpu.0 // Exception 1
  .word interrupt.NMI // Exception 2
  .word interrupt.HardFault // Exception 3
  .word -1 // Exception 4, reserved
  .word -1 // Exception 5, reserved
  .word -1 // Exception 6, reserved
  .word -1 // Exception 7, reserved
  .word -1 // Exception 8, reserved
  .word -1 // Exception 9, reserved
  .word -1 // Exception 10, reserved
  .word interrupt.SVC // Exception 11
  .word -1 // Exception 12, reserved
  .word -1 // Exception 13, reserved
  .word interrupt.PendSV // Exception 14
  .word interrupt.SysTick // Exception 15
//--- Non-Core Vectors
  .word interrupt.TIMER_IRQ_0 // Exception 16, IRQ 0
  .word interrupt.TIMER_IRQ_1 // Exception 17, IRQ 1
  .word interrupt.TIMER_IRQ_2 // Exception 18, IRQ 2
  .word interrupt.TIMER_IRQ_3 // Exception 19, IRQ 3
  .word interrupt.PWM_IRQ_WRAP // Exception 20, IRQ 4
  .word interrupt.USBCTRL_IRQ // Exception 21, IRQ 5
  .word interrupt.XIP_IRQ // Exception 22, IRQ 6
  .word interrupt.PIO0_IRQ_0 // Exception 23, IRQ 7
  .word interrupt.PIO0_IRQ_1 // Exception 24, IRQ 8
  .word interrupt.PIO1_IRQ_0 // Exception 25, IRQ 9
  .word interrupt.PIO1_IRQ_1 // Exception 26, IRQ 10
  .word interrupt.DMA_IRQ_0 // Exception 27, IRQ 11
  .word interrupt.DMA_IRQ_1 // Exception 28, IRQ 12
  .word interrupt.IO_IRQ_BANK0 // Exception 29, IRQ 13
  .word interrupt.IO_IRQ_QSPI // Exception 30, IRQ 14
  .word interrupt.SIO_IRQ_PROC0 // Exception 31, IRQ 15
  .word interrupt.SIO_IRQ_PROC1 // Exception 32, IRQ 16
  .word interrupt.CLOCKS_IRQ // Exception 33, IRQ 17
  .word interrupt.SPI0_IRQ // Exception 34, IRQ 18
  .word interrupt.SPI1_IRQ // Exception 35, IRQ 19
  .word interrupt.UART0_IRQ // Exception 36, IRQ 20
  .word interrupt.UART1_IRQ // Exception 37, IRQ 21
  .word interrupt.ADC_IRQ_FIFO // Exception 38, IRQ 22
  .word interrupt.I2C0_IRQ // Exception 39, IRQ 23
  .word interrupt.I2C1_IRQ // Exception 40, IRQ 24
  .word interrupt.RTC_IRQ // Exception 41, IRQ 25
  .word -1 // Exception 42, reserved, IRQ 26
  .word -1 // Exception 43, reserved, IRQ 27
  .word -1 // Exception 44, reserved, IRQ 28
  .word -1 // Exception 45, reserved, IRQ 29
  .word -1 // Exception 46, reserved, IRQ 30
  .word -1 // Exception 47, reserved, IRQ 31

//--------------------------------------------------------------------------------------------------
