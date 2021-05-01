	.syntax unified
	.cpu cortex-m0plus
	.thumb

//--------------------------------------------------------------------------------------------------
//   UNUSED INTERRUPT (STEPS 01 to 09)
//--------------------------------------------------------------------------------------------------

  .section .text.unused.interrupt, "ax", %progbits

  .align  1
  .global unused.interrupt

unused.interrupt:
  b    unused.interrupt

//--------------------------------------------------------------------------------------------------
