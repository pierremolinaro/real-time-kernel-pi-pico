#include "all-headers.h"

//--------------------------------------------------------------------------------------------------

static const uint32_t BUFFER_SIZE = 200 ;

//--------------------------------------------------------------------------------------------------

class Buffer {
//--- Default constructor
  public: Buffer (void) :
  mBuffer (),
  mCount (0),
  mReadIndex (0),
  mMutex (1),
  mReadSemaphore (0),
  mWriteSemaphore (BUFFER_SIZE) {
  }

//--- append
  public: void append (USER_MODE_ const uint8_t * inObject) {
    mWriteSemaphore.P (MODE) ;
    mMutex.P (MODE) ;
      const uint32_t writeIndex = (mCount + mReadIndex) % BUFFER_SIZE ;
      mBuffer [writeIndex] = inObject ;
      mCount += 1 ;
    mMutex.V (MODE) ;
    mReadSemaphore.V (MODE) ;
  }

//--- remove
  public: const uint8_t * remove (USER_MODE) {
    mReadSemaphore.P (MODE) ;
    mMutex.P (MODE) ;
      const uint8_t * result = mBuffer [mReadIndex] ;
      mCount -= 1 ;
      mReadIndex = (mReadIndex + 1) % BUFFER_SIZE ;
    mMutex.V (MODE) ;
    mWriteSemaphore.V (MODE) ;
    return result ;
  }

//--- Private properties
  private: const uint8_t * mBuffer [BUFFER_SIZE] ;
  private: volatile uint32_t mCount = 0 ;
  private: volatile uint32_t mReadIndex = 0 ;
  private: Semaphore mMutex ;
  private: Semaphore mReadSemaphore ;
  private: Semaphore mWriteSemaphore ;

//--- No copy
  private: Buffer (const Buffer &) = delete ;
  private: Buffer & operator = (const Buffer &) = delete ;
} ;

//--------------------------------------------------------------------------------------------------

static volatile uint32_t gProducerCount = 0 ;
static Buffer gBuffer ;

//--------------------------------------------------------------------------------------------------

static void producerTask (USER_MODE) {
  uint32_t bufferSize = 1 ;
  while (1) {
    if (digitalRead (P4_PUSH_BUTTON)) {
      const uint8_t * object = new uint8_t [bufferSize] ;
      gBuffer.append (MODE_ object) ;
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
    const uint8_t * object = gBuffer.remove (MODE) ;
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
