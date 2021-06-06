
  .syntax unified
  .cpu cortex-m0plus
  .thumb

//-------------------------------------------------------------------------------------------------

  .section  .bss.bootrom.function.address.array, "aw", %nobits
  .align    2
  .global   bootrom.function.address.array

bootrom.function.address.array :
  .space 2 // Index  0: popcount32
  .space 2 // Index  1: reverse32
  .space 2 // Index  2: clz32
  .space 2 // Index  3: ctz32
  .space 2 // Index  4: memset
  .space 2 // Index  5: memset4
  .space 2 // Index  6: memcpy
  .space 2 // Index  7: memcpy44
  .space 2 // Index  8: connect_internal_flash
  .space 2 // Index  9: flash_exit_xip
  .space 2 // Index 10: flash_range_erase
  .space 2 // Index 11: flash_range_program
  .space 2 // Index 12: flash_flush_cache
  .space 2 // Index 13: flash_enter_cmd_xip

//--------------------------------------------------------------------------------------------------
// uint32_t popcount32 (uint32_t value) ;

  .section  .run.in.ram.bootrom.popcount32, "ax", %progbits

  .global bootrom.popcount32
  .type bootrom.popcount32, %function
  .align    2

bootrom.popcount32:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 0]
   bx    r3

//--------------------------------------------------------------------------------------------------
// uint32_t reverse32 (uint32_t value) ;

  .section  .run.in.ram.bootrom.reverse32, "ax", %progbits

  .global bootrom.reverse32
  .type bootrom.reverse32, %function
  .align    2

bootrom.reverse32:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 1]
   bx    r3

//--------------------------------------------------------------------------------------------------
// uint32_t clz32 (uint32_t value) ;

  .section  .run.in.ram.bootrom.clz32, "ax", %progbits

  .global bootrom.clz32
  .type bootrom.clz32, %function
  .align    2

bootrom.clz32:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 2]
   bx    r3

//--------------------------------------------------------------------------------------------------
// uint32_t ctz32 (uint32_t value) ;

  .section  .run.in.ram.bootrom.ctz32, "ax", %progbits

  .global bootrom.ctz32
  .type bootrom.ctz32, %function
  .align    2

bootrom.ctz32:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 3]
   bx    r3

//--------------------------------------------------------------------------------------------------
// uint8_t * memset (uint8_t *ptr, uint8_t c, uint32_t n) ;

  .section  .run.in.ram.bootrom.memset, "ax", %progbits

  .global bootrom.memset
  .type bootrom.memset, %function
  .align    2

bootrom.memset:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 4]
   bx    r3

//--------------------------------------------------------------------------------------------------
// uint32_t * memset4 (uint32_t *ptr, uint8_t c, uint32_t n) ;

  .section  .run.in.ram.bootrom.memset4, "ax", %progbits

  .global bootrom.memset4
  .type bootrom.memset4, %function
  .align    2

bootrom.memset4:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 5]
   bx    r3

//--------------------------------------------------------------------------------------------------
// uint8_t * memcpy (uint8_t *dest, uint8_t *src, uint32_t n) ;

  .section  .run.in.ram.bootrom.memcpy, "ax", %progbits

  .global bootrom.memcpy
  .type bootrom.memcpy, %function
  .align    2

bootrom.memcpy:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 6]
   bx    r3

//--------------------------------------------------------------------------------------------------
// uint8_t * memcpy44 (uint32_t *dest, uint32_t *src, uint32_t n) ;

  .section  .run.in.ram.bootrom.memcpy44, "ax", %progbits

  .global bootrom.memcpy44
  .type bootrom.memcpy44, %function
  .align    2

bootrom.memcpy44:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 7]
   bx    r3

//--------------------------------------------------------------------------------------------------
// void connect_internal_flash (void) ;

  .section  .run.in.ram.bootrom.connect_internal_flash, "ax", %progbits

  .global bootrom.connect_internal_flash
  .type bootrom.connect_internal_flash, %function
  .align    2

bootrom.connect_internal_flash:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 8]
   bx    r3

//--------------------------------------------------------------------------------------------------
// void flash_exit_xip (void) ;

  .section  .run.in.ram.bootrom.flash_exit_xip, "ax", %progbits

  .global bootrom.flash_exit_xip
  .type bootrom.flash_exit_xip, %function
  .align    2

bootrom.flash_exit_xip:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 9]
   bx    r3

//--------------------------------------------------------------------------------------------------
// void flash_range_erase (uint32_t addr, size_t count, uint32_t block_size, uint8_t block_cmd) ;

  .section  .run.in.ram.bootrom.flash_range_erase, "ax", %progbits

  .global bootrom.flash_range_erase
  .type bootrom.flash_range_erase, %function
  .align    2

bootrom.flash_range_erase:
   push  {r3}
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 10]
   mov   r12, r3
   pop   {r3}
   bx    r12

//--------------------------------------------------------------------------------------------------
// void flash_range_program (uint32_t addr, const uint8_t *data, size_t count) ;

  .section  .run.in.ram.bootrom.flash_range_program, "ax", %progbits

  .global bootrom.flash_range_program
  .type bootrom.flash_range_program, %function
  .align    2

bootrom.flash_range_program:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 11]
   bx    r3

//--------------------------------------------------------------------------------------------------
// void flash_flush_cache (void) ;

  .section  .run.in.ram.bootrom.flash_flush_cache, "ax", %progbits

  .global bootrom.flash_flush_cache
  .type bootrom.flash_flush_cache, %function
  .align    2

bootrom.flash_flush_cache:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 12]
   bx    r3

//--------------------------------------------------------------------------------------------------
// void flash_enter_cmd_xip (void) ;

  .section  .run.in.ram.bootrom.flash_enter_cmd_xip, "ax", %progbits

  .global bootrom.flash_enter_cmd_xip
  .type bootrom.flash_enter_cmd_xip, %function
  .align    2

bootrom.flash_enter_cmd_xip:
   ldr   r3, = bootrom.function.address.array
   ldrh  r3, [r3, # 2 * 13]
   bx    r3

//--------------------------------------------------------------------------------------------------


