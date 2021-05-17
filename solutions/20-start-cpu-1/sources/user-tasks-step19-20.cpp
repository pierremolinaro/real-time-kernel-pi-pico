#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gProducerCount = 0 ;
static volatile uint32_t gProducerCountPerSecond = 0 ;
static BoundedBuffer gFirstBuffer ;
static BoundedBuffer gSecondBuffer ;

//--------------------------------------------------------------------------------------------------

static void producerTask (USER_MODE) {
  uint32_t allocatedBufferSize = 1 ;
  while (1) {
    if (digitalRead (P4_PUSH_BUTTON)) {
      uint8_t * object = new uint8_t [allocatedBufferSize] ;
      gFirstBuffer.append (MODE_ object) ;
      gProducerCount += 1 ;
      gProducerCountPerSecond += 1 ;
      allocatedBufferSize += 1 ;
      if (allocatedBufferSize == 50) {
        allocatedBufferSize = 1 ;
      }
    }else{
      waitDuring (MODE_ 10) ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

static void intermediateTask (USER_MODE) {
  const uint32_t SIZE = 20 ;
  const uint8_t * buffer [SIZE] ;
  uint32_t count = 0 ;
  uint32_t readIndex = 0 ;
  while (1) {
    const uint8_t * object = nullptr ;
    if ((count < SIZE) && gFirstBuffer.guarded_remove (MODE_ object)) {
      const uint32_t writeIndex = (count + readIndex) % SIZE ;
      buffer [writeIndex] = object ;
      count += 1 ;
    }else if ((count > 0) && gSecondBuffer.guarded_append (MODE_ buffer [readIndex])) {
      readIndex = (readIndex + 1) % SIZE ;
      count -= 1 ;
    }else{
      guard_waitForChange (MODE) ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gConsumerCount = 0 ;

//--------------------------------------------------------------------------------------------------

static void consumerTask (USER_MODE) {
  while (1) {
    const uint8_t * object = gSecondBuffer.remove (MODE) ;
    delete [] object ;
    gConsumerCount += 1 ;
  }
}

//--------------------------------------------------------------------------------------------------

static void displayTask (USER_MODE) {
  while (1) {
    waitDuring (MODE_ 1000) ;
    gotoXY (MODE_ 0, 0) ;
    printUnsigned (MODE_ gProducerCount) ;
    gotoXY (MODE_ 10, 0) ;
    printSpaces (MODE_ 10) ;
    gotoXY (MODE_ 10, 0) ;
    printUnsigned (MODE_ gProducerCountPerSecond) ;
    gProducerCountPerSecond = 0 ;
    gotoXY (MODE_ 0, 1) ;
    printUnsigned (MODE_ gConsumerCount) ;
    gotoXY (MODE_ 0, 2) ;
    printUnsigned (MODE_ usedRAMByteCount ()) ;
    gotoXY (MODE_ 0, 3) ;
    printUnsigned (MODE_ freeRAMByteCount ()) ;
  }
}

//--------------------------------------------------------------------------------------------------

static uint64_t gStack0 [64] ;
static uint64_t gStack1 [64] ;
static uint64_t gStack2 [64] ;
static uint64_t gStackIntermediateTask [128] ;

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), displayTask) ;
  kernel_createTask (MODE_ gStack1, sizeof (gStack1), producerTask) ;
  kernel_createTask (MODE_ gStack2, sizeof (gStack2), consumerTask) ;
  kernel_createTask (MODE_ gStackIntermediateTask, sizeof (gStackIntermediateTask), intermediateTask) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

