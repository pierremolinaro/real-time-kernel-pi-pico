#include "all-headers.h"

//--------------------------------------------------------------------------------------------------
// Led on Raspberry Pi Pico is connected to GP25 (active high)

void setup0 (void) {
//--- Configure GP25 as output digital port
  padsbank0_hw->io [25] =
    PADS_BANK0_GPIO0_IE_BITS // Input enable
  | PADS_BANK0_GPIO0_DRIVE_VALUE_4MA // Drive strength
  | PADS_BANK0_GPIO0_SCHMITT_BITS // Enable schmitt trigger
  ;
  sio_hw->gpio_oe_set = 1 << 25 ;
  iobank0_hw->io [25].ctrl = 5 ;
}

//--------------------------------------------------------------------------------------------------

void loop0 (void) {
//--- Drive GP25 high --> led is on
  sio_hw->gpio_set = 1 << 25 ;
//--- Wait...
  for (volatile uint32_t i=0 ; i< 500 * 1000 ; i++) {}
//--- Drive GP25 low --> led is off
  sio_hw->gpio_clr = 1 << 25 ;
//--- Wait...
  for (volatile uint32_t i=0 ; i< 1000 * 1000 ; i++) {}
}

//--------------------------------------------------------------------------------------------------
