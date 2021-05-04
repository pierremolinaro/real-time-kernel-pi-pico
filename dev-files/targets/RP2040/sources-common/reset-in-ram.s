	.syntax unified
	.cpu cortex-m0plus
	.thumb

//--------------------------------------------------------------------------------------------------

  .section ".reset.in.ram", "ax", %progbits

  .global ram.entry.point
  .type ram.entry.point, %function

ram.entry.point:
  ldr r0, = __system_stack_end_cpu_0
  mov sp, r0
  ldr r0, = reset.handler.cpu.0
  bx  r0

//--------------------------------------------------------------------------------------------------
