#include "all-headers.h"

static uint32_t ADDRESS_IN_FLASH = 0x100000 ;
static uint32_t gData [64] ;

//--------------------------------------------------------------------------------------------------

void section_functionInRam (SECTION_MODE) {
//--- Ensure that the compiler does not move memory access across this method call
  asm volatile ("" : : : "memory");

//--- Restore all QSPI pad controls to their default state, and connect the SSI to the QSPI pads
//    §2.8.2.1.3., page 161
  bootrom_connect_internal_flash () ;
  sio_hw->gpio_set = 1U << 4 ; // Page 46, DO NOT USE FLASH FUNCTIONS

//--- First set up the SSI for serial-mode operations, then issue the fixed XIP exit sequence
// described in Section 2.8.1.2. Note that the bootrom code uses the IO forcing logic to drive
// the CS pin, which must be cleared before returning the SSI to XIP mode (e.g. by a call to
// _flash_flush_cache). This function configures the SSI with a fixed SCK clock divisor of /6.
  bootrom_flash_exit_xip () ;
  sio_hw->gpio_set = 1U << 5 ; // Page 46, DO NOT USE FLASH FUNCTIONS

//--- Here we can access the flash with bootrom_flash_range_erase, bootrom_flash_range_program

// Erase a count bytes, starting at addr (offset from start of flash). Optionally, pass a block
// erase command e.g. D8h block erase, and the size of the block erased by this command — this
// function will use the larger block erase where possible, for much higher erase speed. addr
// must be aligned to a 4096-byte sector, and count must be a multiple of 4096 bytes.
  bootrom_flash_range_erase (ADDRESS_IN_FLASH, // addr,
                             4096,  // count
                             4096,  // block_size,
                             0xD8) ; // block_cmd

//--- Program data to a range of flash addresses starting at addr (offset from the start of
// flash) and count bytes in size. addr must be aligned to a 256-byte boundary, and count must
// be a multiple of 256.
  for (uint32_t i=0 ; i<64 ; i++) {
    gData [i] = 0x12345600 + i ;
  }
  bootrom_flash_range_program (ADDRESS_IN_FLASH, // addr,
                               (uint8_t *) & gData [0], // data
                               256) ; // count

//--- END OF FLASH ACCESS


//--- Flush and enable the XIP cache. Also clears the IO forcing on QSPI CSn, so that the SSI
//    can drive the flash chip select as normal.
  bootrom_flash_flush_cache () ;
  sio_hw->gpio_set = 1U << 6 ; // Page 46, DO NOT USE FLASH FUNCTIONS

//--- Configure the SSI to generate a standard 03h serial read command, with 24 address bits,
// upon each XIP access. This is a very slow XIP configuration, but is very widely supported.
// The debugger calls this function after performing a flash erase/programming operation, so
// that the freshly-programmed code and data is visible to the debug host, without having to
// know exactly what kind of flash device is connected.
//  bootrom_flash_enter_cmd_xip () ;
  flash_enable_xip_via_boot2 () ;
  sio_hw->gpio_set = 1U << 7 ; // Page 46

//--- Ensure that the compiler does not move memory access across this method call
  asm volatile ("" : : : "memory");
}

//--------------------------------------------------------------------------------------------------

static void displayTask (USER_MODE) {
  functionInRam (MODE) ;
  sio_hw->gpio_set = 1U << 8 ; // Page 46, FROM NOW, FLASH FUNCTIONS CAN BE USED AGAIN
  gotoXY (MODE_ 0, 0) ;
  printHex8 (MODE_ uint32_t (functionInRam)) ;
  gotoXY (MODE_ 10, 0) ;
  printHex8 (MODE_ uint32_t (section_functionInRam)) ;
  gotoXY (MODE_ 0, 1) ;
  printHex8 (MODE_ uint32_t (flash_enable_xip_via_boot2)) ;
  extern uint32_t __flash_enable_xip_via_boot2_in_ram_base_address [2] ;
  volatile uint32_t * p = (volatile uint32_t *) (__flash_enable_xip_via_boot2_in_ram_base_address) ;
  gotoXY (MODE_ 0, 2) ;
  printHex8 (MODE_ uint32_t (*p)) ;
  gotoXY (MODE_ 10, 2) ;
  p ++ ;
  printHex8 (MODE_ uint32_t (*p)) ;
//--- Read flash
  uint32_t * addressInFlash = (uint32_t *) (XIP_BASE + ADDRESS_IN_FLASH) ;
  gotoXY (MODE_ 0, 3) ;
  printHex8 (MODE_ *addressInFlash) ;
  gotoXY (MODE_ 10, 3) ;
  addressInFlash ++ ;
  printHex8 (MODE_ *addressInFlash) ;
}

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [128] ;

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), displayTask) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

