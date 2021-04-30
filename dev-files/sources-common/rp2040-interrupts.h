#pragma once

//--------------------------------------------------------------------------------------------------

#include <stdint.h>

//--------------------------------------------------------------------------------------------------
// INTERRUPTS
//--------------------------------------------------------------------------------------------------

enum class ISRSlot {
  TIMER_IRQ_0 = 0,
  TIMER_IRQ_1 = 1,
  TIMER_IRQ_2 = 2,
  TIMER_IRQ_3 = 3,
  PWM_IRQ_WRAP = 4,
  USBCTRL_IRQ = 5,
  XIP_IRQ = 6,
  PIO0_IRQ_0 = 7,
  PIO0_IRQ_1 = 8,
  PIO1_IRQ_0 = 9,
  PIO1_IRQ_1 = 10,
  DMA_IRQ_0 = 11,
  DMA_IRQ_1 = 12,
  IO_IRQ_BANK0 = 13,
  IO_IRQ_QSPI = 14,
  SIO_IRQ_PROC0 = 15,
  SIO_IRQ_PROC1 = 16,
  CLOCKS_IRQ = 17,
  SPI0_IRQ = 18,
  SPI1_IRQ = 19,
  UART0_IRQ = 20,
  UART1_IRQ = 21,
  ADC_IRQ_FIFO = 22,
  I2C0_IRQ = 23,
  I2C1_IRQ = 24,
  RTC_IRQ = 25
} ;

//--------------------------------------------------------------------------------------------------

inline void NVIC_ENABLE_IRQ (const ISRSlot inInterrupt) {
  const uint32_t it = uint32_t (inInterrupt) ;
//--- ICPR
  *((volatile uint32_t *) 0xE000E280) = 1U << it ;
//--- ISER
  *((volatile uint32_t *) 0xE000E100) = 1U << it ;
}

//--------------------------------------------------------------------------------------------------

inline void NVIC_DISABLE_IRQ (const ISRSlot inInterrupt) {
  const uint32_t it = uint32_t (inInterrupt) ;
//--- ICER
  *((volatile uint32_t *) 0xE000E180) = 1U << it ;
}

//--------------------------------------------------------------------------------------------------
// BITBAND
//--------------------------------------------------------------------------------------------------

// inline void bitband32 (const volatile uint32_t & inRegister, const uint8_t inBit, const bool inValue) {
//   const uint32_t address = ((uint32_t) &inRegister - 0x40000000) * 32 + ((uint32_t) inBit) * 4 + 0x42000000 ;
//   volatile uint32_t * ptr = (volatile uint32_t *) address ;
//   *ptr = (uint32_t) inValue ;
// }

//--------------------------------------------------------------------------------------------------
