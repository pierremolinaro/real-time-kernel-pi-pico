//--------------------------------------------------------------------------------------------------
//   INTERRUPT VECTORS
//--------------------------------------------------------------------------------------------------

	.syntax unified
	.cpu cortex-m0plus
	.thumb

//--------------------------------------------------------------------------------------------------

  .section isr.vectors, "a", %progbits

//--------------------------------------------------------------------------------------------------

  .word __system_stack_end
//--- ARM Core System Handler Vectors
  .word reset.handler // 1
  .word interrupt.NMI // 2
  .word interrupt.HardFault // 3
  .word -1 // 4, reserved
  .word -1 // 5, reserved
  .word -1 // 6, reserved
  .word -1 // 7, reserved
  .word -1 // 8, reserved
  .word -1 // 9, reserved
  .word -1 // 10, reserved
  .word interrupt.SVC // 11
  .word -1 // 12, reserved
  .word -1 // 13, reserved
  .word interrupt.PendSV // 14
  .word interrupt.SysTick // 15
//--- Non-Core Vectors
  .word interrupt.TIMER_IRQ_0 // 16, IRQ 0
  .word interrupt.TIMER_IRQ_1 // 17, IRQ 1
  .word interrupt.TIMER_IRQ_2 // 18, IRQ 2
  .word interrupt.TIMER_IRQ_3 // 19, IRQ 3
  .word interrupt.PWM_IRQ_WRAP // 20, IRQ 4
  .word interrupt.USBCTRL_IRQ // 21, IRQ 5
  .word interrupt.XIP_IRQ // 22, IRQ 6
  .word interrupt.PIO0_IRQ_0 // 23, IRQ 7
  .word interrupt.PIO0_IRQ_1 // 24, IRQ 8
  .word interrupt.PIO1_IRQ_0 // 25, IRQ 9
  .word interrupt.PIO1_IRQ_1 // 26, IRQ 10
  .word interrupt.DMA_IRQ_0 // 27, IRQ 11
  .word interrupt.DMA_IRQ_1 // 28, IRQ 12
  .word interrupt.IO_IRQ_BANK0 // 29, IRQ 13
  .word interrupt.IO_IRQ_QSPI // 30, IRQ 14
  .word interrupt.SIO_IRQ_PROC0 // 31, IRQ 15
  .word interrupt.SIO_IRQ_PROC1 // 32, IRQ 16
  .word interrupt.CLOCKS_IRQ // 33, IRQ 17
  .word interrupt.SPI0_IRQ // 34, IRQ 18
  .word interrupt.SPI1_IRQ // 35, IRQ 19
  .word interrupt.UART0_IRQ // 36, IRQ 20
  .word interrupt.UART1_IRQ // 37, IRQ 21
  .word interrupt.ADC_IRQ_FIFO // 38, IRQ 22
  .word interrupt.I2C0_IRQ // 39, IRQ 23
  .word interrupt.I2C1_IRQ // 40, IRQ 24
  .word interrupt.RTC_IRQ // 41, IRQ 25
  .word -1 // 42, reserved, IRQ 26
  .word -1 // 43, reserved, IRQ 27
  .word -1 // 44, reserved, IRQ 28
  .word -1 // 45, reserved, IRQ 29
  .word -1 // 46, reserved, IRQ 30
  .word -1 // 47, reserved, IRQ 31

//--------------------------------------------------------------------------------------------------
