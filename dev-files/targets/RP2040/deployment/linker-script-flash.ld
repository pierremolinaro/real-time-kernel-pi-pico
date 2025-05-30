/*----------------------------------------------------------------------------*/
/*                                                                            */
/*               Memory                                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/

MEMORY {
  flash       (rx)  : ORIGIN = 0x10000000, LENGTH = 2048k
  global_vars (rwx) : ORIGIN = 0x20000000, LENGTH = 128k
  heap        (rwx) : ORIGIN = 0x20000000 + 128k, LENGTH = 128k
  SRAM4       (rwx) : ORIGIN = 0x20040000, LENGTH = 4k
  SRAM5       (rwx) : ORIGIN = 0x20041000, LENGTH = 4k
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*         Discarded sections                                                 */
/*                                                                            */
/*----------------------------------------------------------------------------*/

SECTIONS {
  /DISCARD/ : {
    *(.gnu.*) ;
    *(.glue_7t);
    *(.glue_7);
    *(.ARM.*);
    *(.comment);
    *(.debug_frame);
    *(.vfp11_veneer);
    *(.v4_bx);
    *(.iplt);
    *(.rel.*);
    *(.igot.plt);
    *(rel.ARM.*);
    *(.start.in.ram) ; /* used only for RAM deployment */
  }
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*            Code                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

SECTIONS {
  .text : {
  /*-------------------- BOOT2 */
    __boot2_in_flash_start = . ;
    KEEP (*(.boot2)) ;
    __boot2_in_flash_end = . ;
  /*-------------------- Vectors */
    __vectors_start_cpu_0 = . ;
    KEEP (*(isr.vectors.cpu.0)) ;
    __vectors_end_cpu_0 = . ;
  /*-------------------- Code */
    __code_start = . ;
    . = ALIGN(4) ;
    *(.text*) ;
    *(.text) ;
    *(text) ;
  /*-------------------- Boot routine array */
    . = ALIGN (4) ;
    __boot_routine_array_start = . ;
    KEEP (*(boot.routine.array)) ;
    . = ALIGN (4) ;
    __boot_routine_array_end = . ;
  /*-------------------- Global C++ object constructor call */
    . = ALIGN (4) ;
    __constructor_array_start = . ;
    KEEP (*(.init_array)) ;
    . = ALIGN (4) ;
    __constructor_array_end = . ;
  /*-------------------- Init routine array */
    . = ALIGN (4) ;
    __init_routine_array_start = . ;
    KEEP (*(init.routine.array)) ;
    . = ALIGN (4) ;
    __init_routine_array_end = . ;
  /*-------------------- Real time interrupt routine array */
    . = ALIGN (4) ;
    __real_time_interrupt_routine_array_start = . ;
    KEEP (*(real.time.interrupt.routine.array)) ;
    . = ALIGN (4) ;
    __real_time_interrupt_routine_array_end = . ;
  /*-------------------- Phase 3 init CPU 1 routine array */
    . = ALIGN (4) ;
    __phase_3_cpu_1_init_routine_array_start = . ;
    KEEP (*(phase.3.cpu.1.init.routine.array)) ;
    . = ALIGN (4) ;
    __phase_3_cpu_1_init_routine_array_end = . ;
  /*-------------------- ROM data */
    . = ALIGN(4);
    *(.rodata*);
    . = ALIGN(4);
  /*-------------------- End */
    __code_end = . ;
  } > flash
}

/*----------------------------------------------------------------------------*/

ASSERT ((__boot2_in_flash_end - __boot2_in_flash_start) == 256,
         "ERROR: Pico second stage bootloader must be 256 bytes in size")

ASSERT ((__vectors_start_cpu_0 % 256) == 0,
         "ERROR: vector table of cpu 0 must be 256-byte aligned")

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*      BSS (uninitialized data)                                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/

SECTIONS {
  .bss : {
    . = ALIGN(4);
    __bss_start = . ;
    * (.bss*) ;
    . = ALIGN(4);
    * (COMMON) ;
    . = ALIGN(4);
    __bss_end = . ;
  } > global_vars
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*      Data (initialized data)                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

SECTIONS {
  .data : AT (__code_end) {
    . = ALIGN (4) ;
    __data_start = . ;
    * (.data*) ;
    __data_end = . ;
  } > global_vars
}


/*----------------------------------------------------------------------------*/

__data_load_start = LOADADDR (.data) ;
__data_load_end   = LOADADDR (.data) + SIZEOF (.data) ;

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                        Boot2                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

SECTIONS {
  .boot2.in.ram : {
    __flash_enable_xip_via_boot2_in_ram_base_address = . ;
    . += 1 ;
    flash.enable.xip.via.boot2 = . ;
    . += 255 ;
  } > SRAM5
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                        Run code in RAM                                     */
/*                                                                            */
/*----------------------------------------------------------------------------*/

SECTIONS {
  .run.in.ram : AT (__data_load_end) {
    . = ALIGN (8) ;
    __run_in_ram_start = . ;
    * (.run.in.ram*) ;
     . = ALIGN (8) ;
    __run_in_ram_end = . ;
  } > SRAM5
}

/*----------------------------------------------------------------------------*/

__run_in_ram_load_start = LOADADDR (.run.in.ram) ;
__run_in_ram_load_end   = LOADADDR (.run.in.ram) + SIZEOF (.run.in.ram) ;

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*            System stacks                                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

SECTIONS {
  .system_stack_cpu_0 : {
    . = ALIGN (8) ;
    __system_stack_start_cpu_0 = . ;
    . += 2048 ;
    . = ALIGN (4) ;
    __system_stack_end_cpu_0 = . ;
    __system_stack_start_cpu_1 = . ;
    . += 2048 ;
    . = ALIGN (4) ;
    __system_stack_end_cpu_1 = . ;
  } > SRAM4
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                Heap                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/

__heap_start = ORIGIN (heap) ;
__heap_end = ORIGIN(heap) + LENGTH(heap) ;

/*----------------------------------------------------------------------------*/
