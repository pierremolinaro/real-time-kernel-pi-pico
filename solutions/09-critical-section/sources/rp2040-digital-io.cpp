#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void pinMode (const DigitalPort inPort, const DigitalMode inMode) {
  if (inPort < DigitalPort::True) {
    const uint32_t portIndex = uint32_t (inPort) ;
//    volatile uint32_t * PADS_BANK0_GPIOn = (volatile uint32_t *) (0x4001C004 + 4 * portIndex) ;
//    volatile uint32_t * IO_BANK0_GPIOn_CTRL = (volatile uint32_t *) (0x40014004 + 8 * portIndex) ;
    switch (inMode) {
    case DigitalMode::OUTPUT :
      padsbank0_hw->io [portIndex] = // Page 321
        PADS_BANK0_GPIO0_SLEWFAST_BITS // Slew Rate Control: fast
      | PADS_BANK0_GPIO0_IE_BITS // Input enable
      | PADS_BANK0_GPIO0_DRIVE_VALUE_4MA // Drive strength
      | PADS_BANK0_GPIO0_SCHMITT_BITS // Enable schmitt trigger
      ;
      sio_hw->gpio_oe_set = 1U << portIndex ; // Enable Output, page 47
      sio_hw->gpio_clr = 1U << portIndex ; // Set output to low level, page 46
      iobank0_hw->io [portIndex].ctrl = 5 ; // Function "SIO"
      break ;
    case DigitalMode::INPUT :
      padsbank0_hw->io [portIndex] = // Page 321
          PADS_BANK0_GPIO0_OD_BITS // Disable Output
        | PADS_BANK0_GPIO0_IE_BITS // Input enable
        | PADS_BANK0_GPIO0_SLEWFAST_BITS // Slew Rate Control: fast
        | PADS_BANK0_GPIO0_SCHMITT_BITS // Enable schmitt trigger
      ;
      sio_hw->gpio_oe_clr = 1U << portIndex ; // Disable output, page 47
      iobank0_hw->io [portIndex].ctrl = 5 ; // Function "SIO"
      break ;
    case DigitalMode::INPUT_PULLDOWN :
      padsbank0_hw->io [portIndex] = // Page 321
          PADS_BANK0_GPIO0_OD_BITS // Disable Output
        | PADS_BANK0_GPIO0_IE_BITS // Input enable
        | PADS_BANK0_GPIO0_SLEWFAST_BITS // Slew Rate Control: fast
        | PADS_BANK0_GPIO0_PDE_BITS // Pull down enable
        | PADS_BANK0_GPIO0_SCHMITT_BITS // Enable schmitt trigger
      ;
      sio_hw->gpio_oe_clr = 1U << portIndex ; // Disable output, page 47
      iobank0_hw->io [portIndex].ctrl = 5 ; // Function "SIO"
      break ;
    case DigitalMode::INPUT_PULLUP :
      padsbank0_hw->io [portIndex] = // Page 321
          PADS_BANK0_GPIO0_OD_BITS // Disable Output
        | PADS_BANK0_GPIO0_IE_BITS // Input enable
        | PADS_BANK0_GPIO0_SLEWFAST_BITS // Slew Rate Control: fast
        | PADS_BANK0_GPIO0_PUE_BITS // Pull up enable
        | PADS_BANK0_GPIO0_SCHMITT_BITS // Enable schmitt trigger
      ;
      sio_hw->gpio_oe_clr = 1U << portIndex ; // Disable output, page 47
      iobank0_hw->io [portIndex].ctrl = 5 ; // Function "SIO"
      break ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

void digitalWrite (const DigitalPort inPort, const bool inValue) {
  if (inPort < DigitalPort::True) {
    const uint32_t portIndex = uint32_t (inPort) ;
    if (inValue) {
      sio_hw->gpio_set = 1U << portIndex ; // Page 46
    }else{
      sio_hw->gpio_clr = 1U << portIndex ; // Page 46
    }
  }
}

//--------------------------------------------------------------------------------------------------

bool digitalRead (const DigitalPort inPort) {
  bool result = inPort == DigitalPort::True ;
  if (inPort < DigitalPort::True) {
    const uint32_t portIndex = uint32_t (inPort) ;
    result = (sio_hw->gpio_in & (1U << portIndex)) != 0 ;
  }
  return result ;
}

//--------------------------------------------------------------------------------------------------

void digitalToggle (const DigitalPort inPort) {
  if (inPort < DigitalPort::True) {
    const uint32_t portIndex = uint32_t (inPort) ;
    sio_hw->gpio_togl = 1U << portIndex ; // Page 46
  }
}

//--------------------------------------------------------------------------------------------------
