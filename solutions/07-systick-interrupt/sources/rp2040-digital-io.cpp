#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void pinMode (const DigitalPort inPort, const DigitalMode inMode) {
  if (inPort < DigitalPort::True) {
    const uint32_t portIndex = uint32_t (inPort) ;
    volatile uint32_t * PADS_BANK0_GPIOn = (volatile uint32_t *) (0x4001C004 + 4 * portIndex) ;
    volatile uint32_t * IO_BANK0_GPIOn_CTRL = (volatile uint32_t *) (0x40014004 + 8 * portIndex) ;
    switch (inMode) {
    case DigitalMode::OUTPUT :
      *PADS_BANK0_GPIOn = // Page 321
        PADS_BANK0_GPIO0_SLEWFAST // Slew Rate Control: fast
      | PADS_BANK0_GPIO0_IE // Input enable
      | PADS_BANK0_GPIO0_DRIVE (1) // Drive strength
      | PADS_BANK0_GPIO0_SCHMITT // Enable schmitt trigger
      ;
      SIO_GPIO_OE_SET = 1U << portIndex ; // Enable Output, page 47
      SIO_GPIO_OUT_CLR = 1U << portIndex ; // Set output to low level, page 46
      *IO_BANK0_GPIOn_CTRL = 5 ; // Function "SIO"
      break ;
    case DigitalMode::INPUT :
      *PADS_BANK0_GPIOn = // Page 321
          PADS_BANK0_GPIO0_OD // Disable Output
        | PADS_BANK0_GPIO0_IE // Input enable
        | PADS_BANK0_GPIO0_SLEWFAST // Slew Rate Control: fast
        | PADS_BANK0_GPIO0_SCHMITT // Enable schmitt trigger
      ;
      SIO_GPIO_OE_CLR = 1U << portIndex ; // Disable output, page 47
      *IO_BANK0_GPIOn_CTRL = 5 ; // Function "SIO"
      break ;
    case DigitalMode::INPUT_PULLDOWN :
      *PADS_BANK0_GPIOn = // Page 321
          PADS_BANK0_GPIO0_OD // Disable Output
        | PADS_BANK0_GPIO0_IE // Input enable
        | PADS_BANK0_GPIO0_SLEWFAST // Slew Rate Control: fast
        | PADS_BANK0_GPIO0_PDE // Pull down enable
        | PADS_BANK0_GPIO0_SCHMITT // Enable schmitt trigger
      ;
      SIO_GPIO_OE_CLR = 1U << portIndex ; // Disable output, page 47
      *IO_BANK0_GPIOn_CTRL = 5 ; // Function "SIO"
      break ;
    case DigitalMode::INPUT_PULLUP :
      *PADS_BANK0_GPIOn = // Page 321
          PADS_BANK0_GPIO0_OD // Disable Output
        | PADS_BANK0_GPIO0_IE // Input enable
        | PADS_BANK0_GPIO0_SLEWFAST // Slew Rate Control: fast
        | PADS_BANK0_GPIO0_PUE // Pull up enable
        | PADS_BANK0_GPIO0_SCHMITT // Enable schmitt trigger
      ;
      SIO_GPIO_OE_CLR = 1U << portIndex ; // Disable output, page 47
      *IO_BANK0_GPIOn_CTRL = 5 ; // Function "SIO"
      break ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

void digitalWrite (const DigitalPort inPort, const bool inValue) {
  if (inPort < DigitalPort::True) {
    const uint32_t portIndex = uint32_t (inPort) ;
    if (inValue) {
      SIO_GPIO_OUT_SET = 1U << portIndex ; // Page 46
    }else{
      SIO_GPIO_OUT_CLR = 1U << portIndex ; // Page 46
    }
  }
}

//--------------------------------------------------------------------------------------------------

bool digitalRead (const DigitalPort inPort) {
  bool result = inPort == DigitalPort::True ;
  if (inPort < DigitalPort::True) {
    const uint32_t portIndex = uint32_t (inPort) ;
    result = (SIO_GPIO_IN & (1U << portIndex)) != 0 ;
  }
  return result ;
}

//--------------------------------------------------------------------------------------------------

void digitalToggle (const DigitalPort inPort) {
  if (inPort < DigitalPort::True) {
    const uint32_t portIndex = uint32_t (inPort) ;
    SIO_GPIO_OUT_XOR = 1U << portIndex ; // Page 46
  }
}

//--------------------------------------------------------------------------------------------------
