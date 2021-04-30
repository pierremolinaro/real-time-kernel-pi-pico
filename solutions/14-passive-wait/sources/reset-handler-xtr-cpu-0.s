	.syntax unified
	.cpu cortex-m0plus
	.thumb

//--------------------------------------------------------------------------------------------------
//  RESET HANDLER (DOUBLE STACK MODE) CPU 0
//--------------------------------------------------------------------------------------------------

//--- This is stack for background task
   BACKGROUND.STACK.SIZE.CPU.0 = 512

	.section	.bss.background.task.stack, "aw", %nobits
  .align	  3   // Stack should be aligned on a 8-byte boundary

background.task.stack.cpu.0:
  .space	BACKGROUND.STACK.SIZE.CPU.0

//--------------------------------------------------------------------------------------------------

  .section	".text", "ax", %progbits

  .global reset.handler.cpu.0
  .type reset.handler.cpu.0, %function

reset.handler.cpu.0: // Cortex M0 boots with interrupts enabled, in Thread mode
//---------------------------------- Run boot, zero bss section, copy data section
  bl    cpu.0.phase3.boot
//---------------------------------- Set PSP: this is stack for background task
  ldr   r0,  =background.task.stack.cpu.0 + BACKGROUND.STACK.SIZE.CPU.0
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
  svc   #0
//---------------------------------- This is the background task: turn off activity led
//  Activity led 0 is connected to GP26
background.task.cpu.0: // Only use R0, R1, R2, R3 and R12. Other registers are not preserved
  ldr   r0, = 0xD0000000 + 0x018 // Address of GPIO_OUT_CLR control register
  ldr   r1, = (1 << 26)   // Port GP26
  str   r1, [r0]         // Turn off
  b     background.task.cpu.0

//--------------------------------------------------------------------------------------------------
//  RESET HANDLER (DOUBLE STACK MODE) CPU 1
//--------------------------------------------------------------------------------------------------

  .section	".text", "ax", %progbits

  .global reset.handler.cpu.1
  .type reset.handler.cpu.1, %function

reset.handler.cpu.1: // Cortex M0 boots with interrupts enabled, in Thread mode
  b  reset.handler.cpu.1

//--------------------------------------------------------------------------------------------------
