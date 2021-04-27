#include "all-headers.h"

//--------------------------------------------------------------------------------------------------
// Led L2 is connected to PORTD:7 (active high)

void setup (USER_MODE) {
//--- Configure Systick
  startSystick (MODE) ;
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

void loop (USER_MODE) {
//--- Drive GP25 high --> led is on
  SIO_GPIO_OUT_SET = 1 << 25 ;
//--- Wait...
  busyWaitDuring (MODE_ 250) ;
//--- Drive GP25 low --> led is off
  SIO_GPIO_OUT_CLR = 1 << 25 ;
//--- Wait...
  busyWaitDuring (MODE_ 750) ;
}

//--------------------------------------------------------------------------------------------------
