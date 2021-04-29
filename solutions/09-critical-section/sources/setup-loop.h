#pragma once

//--------------------------------------------------------------------------------------------------

#include "software-modes.h"

//--------------------------------------------------------------------------------------------------

void setup0 (USER_MODE) asm ("cpu.0.setup") ;

void loop0 (USER_MODE) asm ("cpu.0.loop") ;

//--------------------------------------------------------------------------------------------------

//$section fonction.incrementations

void incrementations (USER_MODE) asm ("fonction.incrementations") ;

void section_incrementations (SECTION_MODE) asm ("section.fonction.incrementations") ;

//--------------------------------------------------------------------------------------------------
