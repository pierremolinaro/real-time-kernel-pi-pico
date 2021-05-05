/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*                                   Memory                                                       */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

MEMORY {
  code_global_vars (rwx) : ORIGIN = 0x20000000, LENGTH = 128k
  heap        (rwx) : ORIGIN = 0x20000000 + 128k, LENGTH = 128k
  SRAM4       (rwx) : ORIGIN = 0x20040000, LENGTH = 4k
  SRAM5       (rwx) : ORIGIN = 0x20041000, LENGTH = 4k
}

/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*                             Discarded sections                                                 */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

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
    *(.boot2) ;  /* used only for FLASH deployment */
  }
}

/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*                                Code                                                            */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

ENTRY(ram.entry.point)

SECTIONS {
  .text : {
  /*-------------------- Entry point in RAM should be at the beginning of the RAM */
    KEEP (*(.reset.in.ram)) ;
  /*-------------------- BSS */
    . = ALIGN(4);
    __bss_start = . ;
    * (.bss*) ;
    . = ALIGN(4);
    * (COMMON) ;
    . = ALIGN(4);
    __bss_end = . ;
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
  /*-------------------- ROM data */
    . = ALIGN(4);
    *(.rodata*);
    . = ALIGN(4);
  /*-------------------- End */
    __code_end = . ;
  } > code_global_vars
}

/*------------------------------------------------------------------------------------------------*/

ASSERT ((__vectors_start_cpu_0 % 256) == 0,
         "ERROR: vector table of cpu 0 must be 256-byte aligned")

/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*                          Data (initialized data)                                               */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

SECTIONS {
  .data : AT (__code_end) {
    . = ALIGN (4) ;
    __data_start = . ;
    * (.data*) ;
    . = ALIGN (4) ;
    __data_end = . ;
  } > code_global_vars
}


/*------------------------------------------------------------------------------------------------*/

__data_load_start = LOADADDR (.data) ;
__data_load_end   = LOADADDR (.data) + SIZEOF (.data) ;

/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*                                System stacks                                                   */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

SECTIONS {
  .system_stack_cpu_0 : {
  /*-------------------- Vectors */
    __vectors_start_cpu_0 = . ;
    KEEP (*(isr.vectors.cpu.0)) ;
    __vectors_end_cpu_0 = . ;
    . = ALIGN (8) ;
    __system_stack_start_cpu_0 = . ;
    . += 4096 - 48 * 4 ;
    . = ALIGN (4) ;
    __system_stack_end_cpu_0 = . ;
  } > SRAM4
}

/*------------------------------------------------------------------------------------------------*/

SECTIONS {
  .system_stack_cpu_1 : {
    . = ALIGN (8) ;
    __system_stack_start_cpu_1 = . ;
    . += 4096 ;
    . = ALIGN (4) ;
    __system_stack_end_cpu_1 = . ;
  } > SRAM5
}

/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*                                    Heap                                                        */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

__heap_start = ORIGIN (heap) ;
__heap_end = ORIGIN(heap) + LENGTH(heap) ;

/*------------------------------------------------------------------------------------------------*/
