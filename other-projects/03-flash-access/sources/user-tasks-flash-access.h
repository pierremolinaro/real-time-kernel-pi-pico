#pragma once

#include "software-modes.h"
#include "run-in-ram.h"

//--------------------------------------------------------------------------------------------------

//$section function.in.ram

void functionInRam (USER_MODE) asm ("function.in.ram") ;

void RUN_IN_RAM section_functionInRam (SECTION_MODE) asm ("section.function.in.ram") ;

//--------------------------------------------------------------------------------------------------

