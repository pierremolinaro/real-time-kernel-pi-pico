#pragma once
#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

//$interrupt-section PWM_IRQ_WRAP
void pwmInterruptServiceRoutine (SECTION_MODE) asm ("interrupt.section.PWM_IRQ_WRAP") ;

//--------------------------------------------------------------------------------------------------
