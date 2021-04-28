#include "all-headers.h"

//--------------------------------------------------------------------------------------------------
// Led on Raspberry Pi Pico is connected to GP25 (active high)

void setup (void) {
//--- Configure Systick
  startSystick () ;
//--- Configure GP25 as output digital port
  PADS_BANK0_GPIO25 =
    PADS_BANK0_GPIO25_IE // Input enable
  | PADS_BANK0_GPIO25_DRIVE (1) // Drive strength
  | PADS_BANK0_GPIO25_SCHMITT // Enable schmitt trigger
  ;
  SIO_GPIO_OE_SET = 1 << 25 ;
  IO_BANK0_GPIO25_CTRL = 5 ;
}

//--------------------------------------------------------------------------------------------------

void loop (void) {
//--- Drive GP25 high --> led is on
  SIO_GPIO_OUT_SET = 1 << 25 ;
//--- Wait...
  busyWaitDuring (250) ;
//--- Drive GP25 low --> led is off
  SIO_GPIO_OUT_CLR = 1 << 25 ;
//--- Wait...
  busyWaitDuring (750) ;
}

//--------------------------------------------------------------------------------------------------
