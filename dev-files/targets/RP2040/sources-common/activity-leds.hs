
//-----------------------------------------------------------------------------
//   ACTIVITY LED 0 ON (only use r0, r1, r2 or r3)
//-----------------------------------------------------------------------------

.macro MACRO_ACTIVITY_LED_0_ON
  ldr   r0, = 0xD0000000 + 0x014 // Address of SIO_GPIO_OUT_SET control register
  ldr   r1, = (1 << 26)  // Port GP26
  str   r1, [r0]         // Turn on
.endm

//-----------------------------------------------------------------------------
//   ACTIVITY LED 0 or 1 ON (only use r0, r1, r2 or r3)
//-----------------------------------------------------------------------------

.macro MACRO_ACTIVITY_LED_0_OR_1_ON
  ldr   r0, = 0xD0000000 + 0x000 // Address of SIO CPUID control register
  ldr   r1, [r0]        // R1 <- 0 for CPU0, 1 for CPU 1
  ldr   r2, = (1 << 26) // Port GP26
  lsls  r2, r2, r1      // R2 <- (1 << 26) for CPU0, (1 << 27) for CPU 1
  adds  r0, # 0x014     // Address of GPIO_OUT_SET control register
  str   r2, [r0]        // Turn on
.endm

//-----------------------------------------------------------------------------
//   ACTIVITY LED 0 OFF (only use r0, r1, r2 or r3)
//-----------------------------------------------------------------------------

.macro MACRO_ACTIVITY_LED_0_OFF
  ldr   r0, = 0xD0000000 + 0x018 // Address of GPIO_OUT_CLR control register
  ldr   r1, = (1 << 26)  // Port GP26
  str   r1, [r0]         // Turn off
.endm

//-----------------------------------------------------------------------------
//   ACTIVITY LED 1 OFF (only use r0, r1, r2 or r3)
//-----------------------------------------------------------------------------

.macro MACRO_ACTIVITY_LED_1_OFF
  ldr   r0, = 0xD0000000 + 0x018 // Address of GPIO_OUT_CLR control register
  ldr   r1, = (1 << 27)  // Port GP27
  str   r1, [r0]         // Turn off
.endm
