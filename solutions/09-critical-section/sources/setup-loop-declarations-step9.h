#pragma once

//--------------------------------------------------------------------------------------------------

#include "software-modes.h"

//--------------------------------------------------------------------------------------------------

//$section fonction.incrementations

void incrementations (USER_MODE) asm ("fonction.incrementations") ;

void section_incrementations (SECTION_MODE) asm ("section.fonction.incrementations") ;

//--------------------------------------------------------------------------------------------------
