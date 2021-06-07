#pragma once

#include <stdint.h>

//--------------------------------------------------------------------------------------------------

uint32_t bootrom_popcount32 (uint32_t value) asm ("bootrom.popcount32") ;

uint32_t bootrom_reverse32 (uint32_t value) asm ("bootrom.reverse32") ;

uint32_t bootrom_clz32 (uint32_t value) asm ("bootrom.clz32") ;

uint32_t bootrom_ctz32 (uint32_t value) asm ("bootrom.ctz32") ;

uint8_t * bootrom_memset (uint8_t *ptr, uint8_t c, uint32_t n) asm ("bootrom.memset") ;

uint32_t * bootrom_memset4 (uint32_t *ptr, uint8_t c, uint32_t n) asm ("bootrom.memset4") ;

uint8_t * bootrom_memcpy (uint8_t *dest, uint8_t *src, uint32_t n) asm ("bootrom.memcpy") ;

uint8_t * bootrom_memcpy44 (uint32_t *dest, uint32_t *src, uint32_t n) asm ("bootrom.memcpy44") ;

void bootrom_connect_internal_flash (void) asm ("bootrom.connect_internal_flash") ;

void bootrom_flash_exit_xip (void) asm ("bootrom.flash_exit_xip") ;

void bootrom_flash_range_erase (uint32_t addr,
                                size_t count,
                                uint32_t block_size,
                                uint8_t block_cmd) asm ("bootrom.flash_range_erase") ;

void bootrom_flash_range_program (uint32_t addr,
                                  const uint8_t *data,
                                  size_t count) asm ("bootrom.flash_range_program") ;

void bootrom_flash_flush_cache (void) asm ("bootrom.flash_flush_cache") ;

void bootrom_flash_enter_cmd_xip (void) asm ("bootrom.flash_enter_cmd_xip") ;

//--------------------------------------------------------------------------------------------------

void flash_enable_xip_via_boot2 (void) asm ("flash.enable.xip.via.boot2") ;

//--------------------------------------------------------------------------------------------------
