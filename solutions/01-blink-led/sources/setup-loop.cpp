#include "all-headers.h"

//--------------------------------------------------------------------------------------------------
// Led on Raspberry Pi Pico is connected to GP25 (active high)

void setup (void) {
  PSM_FRCE_ON = 0x1FFFF ;
  for (volatile uint32_t i=0 ; i< 1 * 1000 * 1000 ; i++) {}
//  while (PSM_DONE != 0x1FFFF) {}
//--- Configure GP25 as output digital port
 // sio_hw->gpio_oe_set = (1 << 25) ;
  SIO_GPIO_OE_SET = 1 << 25 ;
//  iobank0_hw->io[25].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB ;
  IO_BANK0_GPIO25_CTRL = 5 ;
}

//--------------------------------------------------------------------------------------------------

void loop (void) {
//--- Drive GP25 high --> led is on
//  sio_hw->gpio_set = (1 << 25) ;
  SIO_GPIO_OUT_SET = 1 << 25 ;
//--- Wait...
  for (volatile uint32_t i=0 ; i< 1 * 1000 * 1000 ; i++) {}
//--- Drive GP25 low --> led is off
//  sio_hw->gpio_clr = (1 << 25) ;
  SIO_GPIO_OUT_CLR = 1 << 25 ;
//--- Wait...
  for (volatile uint32_t i=0 ; i< 1 * 1000 * 1000 ; i++) {}
}

//--------------------------------------------------------------------------------------------------
