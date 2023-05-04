#include "all-headers.h"

#pragma GCC diagnostic ignored "-Warray-bounds"


//-------------------------------------------------------------------------------------------------

extern uint16_t bootromFunctionAddressArray [14] asm ("bootrom.function.address.array") ;

static const uint32_t popcount32_index             =  0 ;
static const uint32_t reverse32_index              =  1 ;
static const uint32_t clz32_index                  =  2 ;
static const uint32_t ctz32_index                  =  3 ;
static const uint32_t memset_index                 =  4 ;
static const uint32_t memset4_index                =  5 ;
static const uint32_t memcpy_index                 =  6 ;
static const uint32_t memcpy44_index               =  7 ;
static const uint32_t connect_internal_flash_index =  8 ;
static const uint32_t flash_exit_xip_index         =  9 ;
static const uint32_t flash_range_erase_index      = 10 ;
static const uint32_t flash_range_program_index    = 11 ;
static const uint32_t flash_flush_cache_index      = 12 ;
static const uint32_t flash_enter_cmd_xip_index    = 13 ;

//-------------------------------------------------------------------------------------------------

//#define MEMORY(address) *((volatile uint8_t *) (address))

static uint8_t MEMORY (const size_t inAddress) { // Requires GCC diagnostic ignored "-Warray-bounds"
  const uint8_t value = *((volatile uint8_t *) inAddress) ;

  return value ;
}

//-------------------------------------------------------------------------------------------------

static void enterBootromFunction (const uint32_t inFunctionIndex,
                                  const uint8_t inKey0,
                                  const uint8_t inKey1) {
  uint32_t publicFunctionLookupTable = MEMORY (0x15) ;
  publicFunctionLookupTable <<= 8 ;
  publicFunctionLookupTable |= MEMORY (0x14) ;
  uint32_t address = publicFunctionLookupTable ;
//---
  const uint32_t tag = (uint32_t (inKey1) << 8) | uint32_t (inKey0) ;
//---
  bool endReached = false ;
  bool found = false ;
  while (!endReached && !found) {
    const uint32_t currentTag = (MEMORY (address + 1) << 8) | MEMORY (address) ;
    if (currentTag == tag) {
      uint16_t entryPoint = MEMORY (address + 3) ;
      entryPoint <<= 8 ;
      entryPoint |= MEMORY (address + 2) ;
      bootromFunctionAddressArray [inFunctionIndex] = entryPoint ;
    }
    address += 4 ;
    endReached = (currentTag == 0) ;
  }
}

//-------------------------------------------------------------------------------------------------

static void enterAllBootromFunctions (INIT_MODE) {
  enterBootromFunction (popcount32_index, 'P', '3') ;
  enterBootromFunction (reverse32_index, 'R', '3') ;
  enterBootromFunction (clz32_index, 'L', '3') ;
  enterBootromFunction (ctz32_index, 'T', '3') ;
  enterBootromFunction (memset_index, 'M', 'S') ;
  enterBootromFunction (memset4_index, 'S', '4') ; // Error in 2.8.2.1.1 page 161
  enterBootromFunction (memcpy_index, 'M', 'C') ;
  enterBootromFunction (memcpy44_index, 'C', '4') ;
  enterBootromFunction (connect_internal_flash_index, 'I', 'F') ;
  enterBootromFunction (flash_exit_xip_index, 'E', 'X') ;
  enterBootromFunction (flash_range_erase_index, 'R', 'E') ;
  enterBootromFunction (flash_range_program_index, 'R', 'P') ;
  enterBootromFunction (flash_flush_cache_index, 'F', 'C') ;
  enterBootromFunction (flash_enter_cmd_xip_index, 'C', 'X') ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (enterAllBootromFunctions) ;

//--------------------------------------------------------------------------------------------------

