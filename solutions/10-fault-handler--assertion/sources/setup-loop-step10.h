#pragma once

//--------------------------------------------------------------------------------------------------

#include "software-modes.h"

//--------------------------------------------------------------------------------------------------

void setup0 (USER_MODE) asm ("cpu.0.setup") ;

void loop0 (USER_MODE) asm ("cpu.0.loop") ;

//--------------------------------------------------------------------------------------------------

// $interrupt-section SIO_IRQ_PROC0

// void clicInterrupt (SECTION_MODE) asm ("interrupt.section.SIO_IRQ_PROC0") ;

//--------------------------------------------------------------------------------------------------