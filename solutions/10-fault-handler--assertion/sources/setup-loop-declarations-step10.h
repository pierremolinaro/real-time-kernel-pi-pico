#pragma once

//--------------------------------------------------------------------------------------------------

#include "software-modes.h"

//--------------------------------------------------------------------------------------------------

//$interrupt-section IO_IRQ_BANK0

void clicInterrupt (SECTION_MODE) asm ("interrupt.section.IO_IRQ_BANK0") ;

//--------------------------------------------------------------------------------------------------