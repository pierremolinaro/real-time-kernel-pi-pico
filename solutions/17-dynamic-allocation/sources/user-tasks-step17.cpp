#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static const uint32_t BUFFER_SIZE = 200 ;
static const uint8_t * gBuffer [BUFFER_SIZE] ;
static volatile uint32_t gCount = 0 ;
static volatile uint32_t gReadIndex = 0 ;
static Semaphore gMutex (1) ;
static Semaphore gReadSemaphore (0) ;
static Semaphore gWriteSemaphore (BUFFER_SIZE) ;

//--------------------------------------------------------------------------------------------------

static void pushInBuffer (USER_MODE_ const uint8_t * inObject) {
  gWriteSemaphore.P (MODE) ;
  gMutex.P (MODE) ;
    const uint32_t writeIndex = (gCount + gReadIndex) % BUFFER_SIZE ;
    gBuffer [writeIndex] = inObject ;
    gCount += 1 ;
  gMutex.V (MODE) ;
  gReadSemaphore.V (MODE) ;
}

//--------------------------------------------------------------------------------------------------

static const uint8_t * removeFromBuffer (USER_MODE) {
  gReadSemaphore.P (MODE) ;
  gMutex.P (MODE) ;
    const uint8_t * result = gBuffer [gReadIndex] ;
    gCount -= 1 ;
    gReadIndex = (gReadIndex + 1) % BUFFER_SIZE ;
  gMutex.V (MODE) ;
  gWriteSemaphore.V (MODE) ;
  return result ;
}

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gProducerCount = 0 ;

//--------------------------------------------------------------------------------------------------

static void producerTask (USER_MODE) {
  uint32_t bufferSize = 1 ;
  while (1) {
    if (digitalRead (P4_PUSH_BUTTON)) {
      const uint8_t * object = new uint8_t [bufferSize] ;
      pushInBuffer (MODE_ object) ;
      gProducerCount += 1 ;
      bufferSize += 1 ;
      if (bufferSize == 50) {
        bufferSize = 1 ;
      }
    }else{
      waitDuring (MODE_ 1) ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gConsumerCount = 0 ;

//--------------------------------------------------------------------------------------------------

static void consumerTask (USER_MODE) {
  while (1) {
    const uint8_t * object = removeFromBuffer (MODE) ;
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

//--------------------------------------------------------------------------------------------------

static void initTasks (INIT_MODE) {
  kernel_createTask (MODE_ gStack0, sizeof (gStack0), displayTask) ;
  kernel_createTask (MODE_ gStack1, sizeof (gStack1), producerTask) ;
  kernel_createTask (MODE_ gStack2, sizeof (gStack2), consumerTask) ;
}

//--------------------------------------------------------------------------------------------------

MACRO_INIT_ROUTINE (initTasks) ;

//--------------------------------------------------------------------------------------------------

