#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

void pinMode (const DigitalPort inPort, const DigitalMode inMode) {
  const uint32_t portIndex = uint32_t (inPort) ;
  const uint32_t ctrl = 5 ; // Function "SIO"
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
    iobank0_hw->io [portIndex].ctrl = ctrl ; // Function "SIO" (page 268)
    break ;
  case DigitalMode::INPUT :
    padsbank0_hw->io [portIndex] = // Page 321
        PADS_BANK0_GPIO0_OD_BITS // Disable Output
      | PADS_BANK0_GPIO0_IE_BITS // Input enable
      | PADS_BANK0_GPIO0_SLEWFAST_BITS // Slew Rate Control: fast
      | PADS_BANK0_GPIO0_SCHMITT_BITS // Enable schmitt trigger
    ;
    sio_hw->gpio_oe_clr = 1U << portIndex ; // Disable output, page 47
    iobank0_hw->io [portIndex].ctrl = ctrl ; // Function "SIO" (page 268)
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
    iobank0_hw->io [portIndex].ctrl = ctrl ; // Function "SIO" (page 268)
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
    iobank0_hw->io [portIndex].ctrl = ctrl ; // Function "SIO" (page 268)
    break ;
  }
}

//--------------------------------------------------------------------------------------------------

void digitalWrite (const DigitalPort inPort, const bool inValue) {
  const uint32_t portIndex = uint32_t (inPort) ;
  if (inValue) {
    sio_hw->gpio_set = 1U << portIndex ; // Page 46
  }else{
    sio_hw->gpio_clr = 1U << portIndex ; // Page 46
  }
}

//--------------------------------------------------------------------------------------------------

bool digitalRead (const DigitalPort inPort) {
  const uint32_t portIndex = uint32_t (inPort) ;
  return (sio_hw->gpio_in & (1U << portIndex)) != 0 ;
}

//--------------------------------------------------------------------------------------------------

void digitalToggle (const DigitalPort inPort) {
  const uint32_t portIndex = uint32_t (inPort) ;
  sio_hw->gpio_togl = 1U << portIndex ; // Page 46
}

//--------------------------------------------------------------------------------------------------
//  From gpio_acknowledge_irq, pico-sdk-master/src/rp2_common/hardware_gpio/gpio.c
//--------------------------------------------------------------------------------------------------

void gpioAcknowledgeInterrupt (const DigitalPort inPort, const uint32_t inEvents) {
  const uint32_t portIndex = uint32_t (inPort) ;
  const uint32_t events = inEvents << (4 * (portIndex % 8)) ;
  iobank0_hw->intr [portIndex / 8] = events ;
}

//--------------------------------------------------------------------------------------------------
//  From gpio_set_irq_enabled, pico-sdk-master/src/rp2_common/hardware_gpio/gpio.c
//--------------------------------------------------------------------------------------------------

void gpioEnableInterrupt (const DigitalPort inPort, const uint32_t inEvents, const bool inEnable) {
//--- Separate mask/force/status per-core, so check which core called, and
//    set the relevant IRQ controls.
  io_irq_ctrl_hw_t * irq_ctrl_base = (get_core_num () == 1)
    ? & (iobank0_hw->proc1_irq_ctrl)
    : & (iobank0_hw->proc0_irq_ctrl)
  ;
//--- Clear stale events which might cause immediate spurious handler entry
  gpioAcknowledgeInterrupt (inPort, inEvents) ;
//--- Enable or disable irq
  const uint32_t portIndex = uint32_t (inPort) ;
  const uint32_t events = inEvents << (4 * (portIndex % 8)) ;
  io_rw_32 * en_reg = & (irq_ctrl_base->inte [portIndex / 8]) ; // Page 273

  if (inEnable) {
    *en_reg |= events ; // hw_set_bits (en_reg, events);
  }else{
    *en_reg &= ~ events ; //hw_clear_bits (en_reg, events);
  }
}

//--------------------------------------------------------------------------------------------------
