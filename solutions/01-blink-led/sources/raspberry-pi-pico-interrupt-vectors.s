@———————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
@   INTERRUPT VECTORS
@———————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

	.syntax unified
	.cpu cortex-m0plus
	.thumb

@———————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

  .section isr.vectors, "a", %progbits

@———————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

  .word __system_stack_end
@--- ARM Core System Handler Vectors
  .word reset.handler @ 1
  .word interrupt.NMI @ 2
  .word interrupt.HardFault @ 3
  .word -1 @ 4
  .word -1 @ 5
  .word -1 @ 6
  .word -1 @ 7, reserved
  .word -1 @ 8, reserved
  .word -1 @ 9, reserved
  .word -1 @ 10, reserved
  .word interrupt.SVC @ 11
  .word -1 @ 12
  .word -1 @ 13, reserved
  .word interrupt.PendSV @ 14
  .word interrupt.SysTick @ 15
@--- Non-Core Vectors
  .word interrupt.TIMER_IRQ_0 @ 16
  .word interrupt.TIMER_IRQ_1 @ 17
  .word interrupt.TIMER_IRQ_2 @ 18
  .word interrupt.TIMER_IRQ_3 @ 19
  .word interrupt.PWM_IRQ_WRAP @ 20
  .word interrupt.USBCTRL_IRQ @ 21
  .word interrupt.XIP_IRQ @ 22
  .word interrupt.PIO0_IRQ_0 @ 23
  .word interrupt.PIO0_IRQ_1 @ 24
  .word interrupt.PIO1_IRQ_0 @ 25
  .word interrupt.PIO1_IRQ_1 @ 26
  .word interrupt.DMA_IRQ_0 @ 27
  .word interrupt.DMA_IRQ_1 @ 28
  .word interrupt.IO_IRQ_BANK0 @ 29
  .word interrupt.IO_IRQ_QSPI @ 30
  .word interrupt.SIO_IRQ_PROC0 @ 31
  .word interrupt.SIO_IRQ_PROC1 @ 32
  .word interrupt.CLOCKS_IRQ @ 33
  .word interrupt.SPI0_IRQ @ 34
  .word interrupt.SPI1_IRQ @ 35
  .word interrupt.UART0_IRQ @ 36
  .word interrupt.UART1_IRQ @ 37
  .word interrupt.ADC_IRQ_FIFO @ 38
  .word interrupt.I2C0_IRQ @ 39
  .word interrupt.I2C1_IRQ @ 40
  .word interrupt.RTC_IRQ @ 41
  .word -1 @ 42, reserved
  .word -1 @ 43, reserved
  .word -1 @ 44, reserved
  .word -1 @ 45, reserved
  .word -1 @ 46, reserved
  .word -1 @ 47, reserved

@———————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

BINARY_INFO_MARKER_START = 0x7188EBF2
BINARY_INFO_MARKER_END = 0xE71AA390

@———————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

  .section .binary_info_header, "ax", %progbits
  .global __binary_info_start
  .global __binary_info_end
  .global data_cpy_table

binary_info_header:
.word BINARY_INFO_MARKER_START
.word __binary_info_start
.word __binary_info_end
.word data_cpy_table // we may need to decode pointers that are in RAM at runtime.
.word BINARY_INFO_MARKER_END

  .section .text, "ax", %progbits

__binary_info_start:
__binary_info_end:
data_cpy_table:
  .word 0

@———————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
