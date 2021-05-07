#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#---------------------------------------------------------------------------------------------------

def interruptNames () :
  result = {}
  result ["NMI"] = 2
  result ["HardFault"] = 3
  result ["SVC"] = 11
  result ["DebugMonitor"] = 12
  result ["PendSV"] = 14
  result ["SysTick"] = 15
  result ["TIMER_IRQ_0"] = 16
  result ["TIMER_IRQ_1"] = 17
  result ["TIMER_IRQ_2"] = 18
  result ["TIMER_IRQ_3"] = 19
  result ["PWM_IRQ_WRAP"] = 20
  result ["USBCTRL_IRQ"] = 21
  result ["XIP_IRQ"] = 22
  result ["PIO0_IRQ_0"] = 23
  result ["PIO0_IRQ_1"] = 24
  result ["PIO1_IRQ_0"] = 25
  result ["PIO1_IRQ_1"] = 26
  result ["DMA_IRQ_0"] = 27
  result ["DMA_IRQ_1"] = 28
  result ["IO_IRQ_BANK0"] = 29
  result ["IO_IRQ_QSPI"] = 30
  result ["SIO_IRQ_PROC0"] = 31
  result ["SIO_IRQ_PROC1"] = 32
  result ["CLOCKS_IRQ"] = 33
  result ["SPI0_IRQ"] = 34
  result ["SPI1_IRQ"] = 35
  result ["UART0_IRQ"] = 36
  result ["UART1_IRQ"] = 37
  result ["ADC_IRQ_FIFO"] = 38
  result ["I2C0_IRQ"] = 39
  result ["I2C1_IRQ"] = 40
  result ["RTC_IRQ"] = 41
  return result

#---------------------------------------------------------------------------------------------------
