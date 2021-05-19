	.syntax unified
	.cpu cortex-m0plus
	.thumb

//---------------------------------------------------------------------------------------------------
//   INCLUDE ACTIVITY LEDS MACROS
//---------------------------------------------------------------------------------------------------

.include "activity-leds.hs"

//--------------------------------------------------------------------------------------------------
//  RESET HANDLER (DOUBLE STACK MODE) CPU 0
//--------------------------------------------------------------------------------------------------

//--- This is stack for background task
   BACKGROUND.STACK.SIZE.CPU.1 = 32

	.section	.bss.background.task.stack, "aw", %nobits
  .align	  3   // Stack should be aligned on a 8-byte boundary

background.task.stack.cpu.1:
  .space	BACKGROUND.STACK.SIZE.CPU.1

//--------------------------------------------------------------------------------------------------

  .section	".text", "ax", %progbits

  .global reset.handler.cpu.1
  .type reset.handler.cpu.1, %function

reset.handler.cpu.1: // Cortex M0 boots with interrupts enabled, in Thread mode
//---------------------------------- Set PSP: this is stack for background task
  ldr   r0,  =background.task.stack.cpu.1 + BACKGROUND.STACK.SIZE.CPU.1
  msr   psp, r0
//---------------------------------- Set CONTROL register
// bit 0 : 0 -> Thread mode has privileged access, 1 -> Thread mode has unprivileged access
// bit 1 : 0 -> Use SP_main as the current stack, 1 -> In Thread mode, use SP_process as the current stack
  movs  r2, #2
  msr   CONTROL, r2
//--- Software must use an ISB barrier instruction to ensure a write to the CONTROL register
// takes effect before the next instruction is executed.
  isb
//---------------------------------- Run init routines, from SVC handler
  svc   #1
//---------------------------------- This is the background task: turn off activity led
//  Activity led 1 is connected to GP27
background.task.cpu.1: // Only use R0, R1, R2, R3 and R12. Other registers are not preserved
  MACRO_ACTIVITY_LED_1_OFF
  wfi
  b     background.task.cpu.1

//--------------------------------------------------------------------------------------------------
