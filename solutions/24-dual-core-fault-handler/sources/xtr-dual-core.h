#pragma once

//--------------------------------------------------------------------------------------------------

#include "software-modes.h"

//--------------------------------------------------------------------------------------------------
//  SIO Interrupt
//--------------------------------------------------------------------------------------------------

//$interrupt-service SIO_IRQ_PROC0
void sio0InterruptServiceRoutine (IRQ_MODE) asm ("interrupt.service.SIO_IRQ_PROC0") ;

//$interrupt-service SIO_IRQ_PROC1
void sio1InterruptServiceRoutine (IRQ_MODE) asm ("interrupt.service.SIO_IRQ_PROC1") ;

//--------------------------------------------------------------------------------------------------
