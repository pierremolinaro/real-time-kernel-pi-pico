#include "all-headers.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#define ENABLE_XTR_ASSERTIONS

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifdef ENABLE_XTR_ASSERTIONS
  #define XTR_ASSERT_NON_NULL_POINTER(ptr) assertNonNullPointer (ptr, __FILE__, __LINE__) ;
#else
  #define XTR_ASSERT_NON_NULL_POINTER(ptr)
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#ifdef ENABLE_XTR_ASSERTIONS
  #define XTR_ASSERT(condition,value) assertion (condition, value, __FILE__, __LINE__) ;
#else
  #define XTR_ASSERT(condition,value)
#endif

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//    CORTEX-M4 EXCEPTION FRAME (WITHOUT FLOATING-POINT STORAGE)                                                       *
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//                                                                                                                     *
//         PSP+32 -> |                            |                                                                    *
//                   |----------------------------| \                                                                  *
//         PSP+28 -> | xPSR                       |  |                                                                 *
//                   |----------------------------|  |                                                                 *
//         PSP+24 -> | PC (after SVC instruction) |  |                                                                 *
//                   |----------------------------|  |                                                                 *
//         PSP+20 -> | LR                         |  |                                                                 *
//                   |----------------------------|  |                                                                 *
//         PSP+16 -> | R12                        |  |  Saved by interrupt response                                    *
//                   |----------------------------|  |                                                                 *
//         PSP+12 -> | R3                         |  |                                                                 *
//                   |----------------------------|  |                                                                 *
//         PSP+8  -> | R2                         |  |                                                                 *
//                   |----------------------------|  |                                                                 *
//         PSP+4  -> | R1                         |  |                                                                 *
//                   |----------------------------|  |                                                                 *
//   /---- PSP ----> | R0                         |  |                                                                 *
//   |               |----------------------------| /                                                                  *
//   |                                                                                                                 *
//   |                                               *---------------------*
//   |                                               | LR return code      | +36 [ 9]
//   |                                               *---------------------*
//   \-----------------------------------------------| R13 (PSP)           | +32 [ 8]
//                                                   *---------------------*
//                                                   | R11                 | +28 [ 7]
//                                                   *---------------------*
//                                                   | R10                 | +24 [ 6]
//                                                   *---------------------*
//                                                   | R9                  | +20 [ 5]
//                                                   *---------------------*
//                                                   | R8                  | +16 [ 4]
//                                                   *---------------------*
//                                                   | R7                  | +12 [ 3]
//                                                   *---------------------*
//                                                   | R6                  | + 8 [ 2]
//                                                   *---------------------*
//                                                   | R5                  | + 4 [ 1]
//  *---------------------------------------*        *---------------------*
//  | var.running.task.control.block.ptr    +------> | R4                  | + 0 [ 0]
//  *---------------------------------------*        *---------------------*
//
//----------------------------------------------------------------------------------------------------------------------

typedef struct {
  uint32_t mR0 ;
  uint32_t mR1 ;
  uint32_t mR2 ;
  uint32_t mR3 ;
  uint32_t mR12 ;
  uint32_t mLR ;
  uint32_t mPC ;
  uint32_t mXPSR ;
} ExceptionFrame_without_floatingPointStorage ;

//----------------------------------------------------------------------------------------------------------------------

typedef struct {
  uint32_t mR4 ;
  uint32_t mR5 ;
  uint32_t mR6 ;
  uint32_t mR7 ;
  uint32_t mR8 ;
  uint32_t mR9 ;
  uint32_t mR10 ;
  uint32_t mR11 ;
  ExceptionFrame_without_floatingPointStorage * mPSP ; // R13
  uint32_t mLR_RETURN_CODE ;
} TaskContext ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void kernel_set_task_context (INIT_MODE_
                                     TaskContext & ioTaskContext,
                                     const uint32_t inStackBufferAddress,
                                     const uint32_t inStackBufferSize,
                                     RoutineTaskType inTaskRoutine) {
//--- Initialize LR
  ioTaskContext.mLR_RETURN_CODE = 0xFFFFFFFD ;
//--- Stack Pointer initial value
  uint32_t initialTopOfStack = inStackBufferAddress + inStackBufferSize ;
  initialTopOfStack -= sizeof (ExceptionFrame_without_floatingPointStorage) ;
//--- Initialize SP
  auto ptr = (ExceptionFrame_without_floatingPointStorage *) initialTopOfStack ;
  ioTaskContext.mPSP = ptr ;
//--- Initialize PC
  ptr->mPC = (uint32_t) inTaskRoutine ;
//--- Initialize CPSR
  ptr->mXPSR = 1 << 24 ; // Thumb bit
//--- Self termination
  ptr->mLR = (uint32_t) taskSelfTerminates ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   T A S K    C O N T R O L    B L O C K                                                                             *
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

typedef struct TaskControlBlock {
//--- Context buffer
  TaskContext mTaskContext ; // SHOULD BE THE FIRST FIELD
//--- This field is used for deadline
  uint32_t mDeadline ;
//--- Task blocking list (nullptr if task is not blocked)
  TaskList * mBlockingList ;
//--- Task index
  uint8_t mTaskIndex ;
//--- User result
  bool mUserResult ;
} TaskControlBlock ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static TaskControlBlock gTaskDescriptorArray [TASK_COUNT] ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

TaskControlBlock * descriptorPointerForTaskIndex (const uint8_t inTaskIndex) {
  XTR_ASSERT (inTaskIndex < TASK_COUNT, inTaskIndex) ;
  return & gTaskDescriptorArray [inTaskIndex] ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint8_t indexForDescriptorTask (const TaskControlBlock * inTaskPtr) { // should be not nullptr
  XTR_ASSERT_NON_NULL_POINTER (inTaskPtr) ;
  return inTaskPtr->mTaskIndex ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   RUNNING TASK DESCRIPTOR POINTER
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

TaskControlBlock * gRunningTaskControlBlockPtr asm ("var.running.task.control.block.ptr") ; // Shared with assembly code

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   SCHEDULER
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static TaskList gReadyTaskList ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void kernel_makeNoTaskRunning (KERNEL_MODE) {
   gRunningTaskControlBlockPtr = nullptr ; // No running task
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void kernel_makeTaskReady (IRQ_MODE_ TaskControlBlock * inTaskPtr, const bool inUserResult) {
  XTR_ASSERT_NON_NULL_POINTER (inTaskPtr) ;
  gReadyTaskList.enterTask (MODE_ inTaskPtr) ;
  inTaskPtr->mUserResult = inUserResult ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void kernelSelectTaskToRun (IRQ_MODE) asm ("kernel.select.task.to.run") ; // Invoked from assembly

void kernelSelectTaskToRun (IRQ_MODE) {
  if (gRunningTaskControlBlockPtr != nullptr) {
    gReadyTaskList.enterTask (MODE_ gRunningTaskControlBlockPtr) ;
  }
  gRunningTaskControlBlockPtr = gReadyTaskList.removeFirstTask (MODE) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   kernel_createTask
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static uint8_t gTaskIndex ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void kernel_createTask (INIT_MODE_
                        uint64_t * inStackBufferAddress,
                        uint32_t inStackBufferSize,
                        RoutineTaskType inTaskRoutine) {
  XTR_ASSERT (gTaskIndex < TASK_COUNT, gTaskIndex) ;
  TaskControlBlock * taskControlBlockPtr = & gTaskDescriptorArray [gTaskIndex] ;
  taskControlBlockPtr->mTaskIndex = gTaskIndex ;
//--- Initialize properties
//    As gTaskDescriptorArray is in bss, all properties are by default initialized to binary 0
//   taskControlBlockPtr->mDeadline = 0 ; // statically initialized to 0
//   taskControlBlockPtr->mUserResult = false ; // statically initialized to false
//   taskControlBlockPtr->mGuardState = GUARD_EVALUATING_OR_OUTSIDE ; // statically initialized to GUARD_EVALUATING_OR_OUTSIDE
//   taskControlBlockPtr->mGuardDescriptor.mCount = 0 ; // statically initialized to 0
//--- Initialize Context
  kernel_set_task_context (MODE_
                           taskControlBlockPtr->mTaskContext,
                           (uint32_t) inStackBufferAddress,
                           inStackBufferSize,
                           inTaskRoutine) ;
//--- Make task ready
  kernel_makeTaskReady (MODE_ taskControlBlockPtr, true) ;
//---
  gTaskIndex += 1 ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  TASK SELF TERMINATES
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void service_taskSelfTerminates (KERNEL_MODE) {
  kernel_makeNoTaskRunning (MODE) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  USER RESULT
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void kernel_setUserResult (KERNEL_MODE_ const bool inUserResult) {
  XTR_ASSERT_NON_NULL_POINTER (gRunningTaskControlBlockPtr) ;
  gRunningTaskControlBlockPtr->mUserResult = inUserResult ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

bool getUserResult (USER_MODE) {
  XTR_ASSERT_NON_NULL_POINTER (gRunningTaskControlBlockPtr) ;
  return gRunningTaskControlBlockPtr->mUserResult ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   DEADLINE LIST
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static TaskList gDeadlineWaitingTaskList ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void kernel_blockOnDeadline (KERNEL_MODE_ const uint32_t inDeadline) {
  XTR_ASSERT_NON_NULL_POINTER (gRunningTaskControlBlockPtr) ;
//--- Insert in deadline list
  gRunningTaskControlBlockPtr->mDeadline = inDeadline ;
  gDeadlineWaitingTaskList.enterTask (MODE_ gRunningTaskControlBlockPtr) ;
//--- Block task
  kernel_makeNoTaskRunning (MODE) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void irq_makeTasksReadyFromCurrentDate (IRQ_MODE_ const uint32_t inCurrentDate) {
  TaskList::Iterator iterator (MODE_ gDeadlineWaitingTaskList) ;
  TaskControlBlock * taskPtr ;
  while ((taskPtr = iterator.nextTask (MODE))) {
    if (inCurrentDate >= taskPtr->mDeadline) {
    //--- Remove task from blocking list
      if (nullptr != taskPtr->mBlockingList) {
        taskPtr->mBlockingList->removeTask (MODE_ taskPtr) ;
        taskPtr->mBlockingList = nullptr ;
      }
    //--- Remove task from deadline list
      gDeadlineWaitingTaskList.removeTask (MODE_ taskPtr) ;
    //--- Make task ready
      kernel_makeTaskReady (MODE_ taskPtr, false) ;
    }
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

MACRO_REAL_TIME_ISR (irq_makeTasksReadyFromCurrentDate) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  SYNCHRONIZATION
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void kernel_blockRunningTaskInList (KERNEL_MODE_ TaskList & ioWaitingList) {
  XTR_ASSERT_NON_NULL_POINTER (gRunningTaskControlBlockPtr) ;
//--- Insert in task list
  ioWaitingList.enterTask (MODE_ gRunningTaskControlBlockPtr) ;
//--- Block task
  kernel_makeNoTaskRunning (MODE) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

bool irq_makeTaskReadyFromList (IRQ_MODE_ TaskList & ioWaitingList) {
  TaskControlBlock * taskPtr = ioWaitingList.removeFirstTask (MODE) ;
  const bool found = taskPtr != nullptr ;
  if (found) {
    taskPtr->mBlockingList = nullptr ;
  //--- Remove from deadline list
    gDeadlineWaitingTaskList.removeTask (MODE_ taskPtr) ;
  //--- Make task ready
    kernel_makeTaskReady (MODE_ taskPtr, true) ;
  }
  return found ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  SYNCHRONIZATION AND DEADLINE
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void kernel_blockRunningTaskInListAndDeadline (KERNEL_MODE_ TaskList & ioWaitingList, const uint32_t inDeadline) {
  XTR_ASSERT_NON_NULL_POINTER (gRunningTaskControlBlockPtr) ;
//--- Insert in task list
  ioWaitingList.enterTask (MODE_ gRunningTaskControlBlockPtr) ;
  gRunningTaskControlBlockPtr->mBlockingList = & ioWaitingList ;
//--- Insert in deadline list
  gRunningTaskControlBlockPtr->mDeadline = inDeadline ;
  gDeadlineWaitingTaskList.enterTask (MODE_ gRunningTaskControlBlockPtr) ;
//--- Block task
  kernel_makeNoTaskRunning (MODE) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
