#pragma once

//--------------------------------------------------------------------------------------------------

#include "software-modes.h"

//--------------------------------------------------------------------------------------------------
//   Task routine type
//--------------------------------------------------------------------------------------------------

typedef void (* RoutineTaskType) (USER_MODE) ;

//--------------------------------------------------------------------------------------------------
//   kernel_createTask
//--------------------------------------------------------------------------------------------------

void kernel_createTask (INIT_MODE_
                        uint64_t * inStackBufferAddress,
                        uint32_t inStackBufferSize,
                        RoutineTaskType inTaskRoutine) ;

//--------------------------------------------------------------------------------------------------

struct TaskControlBlock ;

TaskControlBlock * descriptorPointerForTaskIndex (const uint8_t inTaskIndex) ;

uint8_t indexForDescriptorTask (const TaskControlBlock * inTaskPtr) ; // should be not nullptr

//--------------------------------------------------------------------------------------------------
//  TASK SELF TERMINATES
//--------------------------------------------------------------------------------------------------

//$service task.self.terminates

void taskSelfTerminates (USER_MODE) asm ("task.self.terminates") ;

void service_taskSelfTerminates (KERNEL_MODE) asm ("service.task.self.terminates") ;

//--------------------------------------------------------------------------------------------------
//  DEADLINE
//--------------------------------------------------------------------------------------------------

void kernel_blockOnDeadline (KERNEL_MODE_ const uint32_t inDeadline) ;

//--------------------------------------------------------------------------------------------------
//  SYNCHRONIZATION
//--------------------------------------------------------------------------------------------------

class TaskList ;

void kernel_blockRunningTaskInList (KERNEL_MODE_ TaskList & ioWaitingList) ;

bool irq_makeTaskReadyFromList (IRQ_MODE_ TaskList & ioWaitingList) ;

//--------------------------------------------------------------------------------------------------
//  USER RESULT
//--------------------------------------------------------------------------------------------------

void kernel_setUserResult (KERNEL_MODE_ const bool inUserResult) ;

bool getUserResult (USER_MODE) asm ("get.user.result") ;

//--------------------------------------------------------------------------------------------------
//  BLOCK RUNNING TASK IN LIST AND DEADLINE
//--------------------------------------------------------------------------------------------------

void kernel_blockRunningTaskInListAndDeadline (KERNEL_MODE_
                                               TaskList & ioWaitingList,
                                               const uint32_t inDeadline) ;

//--------------------------------------------------------------------------------------------------
//  GUARDS
//--------------------------------------------------------------------------------------------------

// GUARD_EVALUATING_OR_OUTSIDE should be the first constant
typedef enum {GUARD_EVALUATING_OR_OUTSIDE, GUARD_DID_CHANGE, GUARD_WAITING_FOR_CHANGE} GuardState ;

//--------------------------------------------------------------------------------------------------

class GuardList {
//--- Default constructor
  public: GuardList (void) : mGuardList (0) {}

//--- Enter task
  public: void enterTask (SECTION_MODE_ TaskControlBlock * inTask) ;

//--- Remove first task
  public: TaskControlBlock * removeFirstTask (SECTION_MODE) ;

//--- Remove task
  public: void removeTask (SECTION_MODE_ TaskControlBlock * inTask) ;

//--- Private property
  private: uint32_t mGuardList ;

//--- No copy
  private: GuardList (const GuardList &) = delete ;
  private: GuardList & operator = (const GuardList &) = delete ;
} ;

//--------------------------------------------------------------------------------------------------

void guard_handleWaitUntil (SECTION_MODE_ const uint32_t inDeadline, const bool inAccepted) ;

//--------------------------------------------------------------------------------------------------

void guard_handleCommand (SECTION_MODE_ GuardList & ioGuardList, const bool inAccepted) ;

//--------------------------------------------------------------------------------------------------

//$section guard.boolean.expression

bool guard_booleanExpression (USER_MODE_ const bool inAccepte) asm ("guard.boolean.expression") ;

bool section_guard_booleanExpression (SECTION_MODE_ const bool inAccepted) asm ("section.guard.boolean.expression") ;

//--------------------------------------------------------------------------------------------------

bool guard_waitForChange (USER_MODE) ;

//--------------------------------------------------------------------------------------------------

//$section guard.wait.until
bool guard_waitUntil (USER_MODE_ const uint32_t inDeadlineMS) asm ("guard.wait.until") ;

bool kernel_guard_waitUntil (SECTION_MODE_ const uint32_t inDeadlineMS)  asm ("section.guard.wait.until") ;

//--------------------------------------------------------------------------------------------------
//  GUARDS    (internal commands )
//--------------------------------------------------------------------------------------------------

//$service guard.internal.wait.for.change

void internalWaitForGuardChange (USER_MODE) asm ("guard.internal.wait.for.change") ;

void kernel_internalWaitForGuardChange (KERNEL_MODE) asm ("service.guard.internal.wait.for.change") ;

//--------------------------------------------------------------------------------------------------

void guard_blockingStateDidChange (IRQ_MODE_ GuardList & ioGuardList) ;

//--------------------------------------------------------------------------------------------------
