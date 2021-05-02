#! /usr/bin/env python
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

import sys, interrupt_names_raspberry_pi_pico

#---------------------------------------------------------------------------------------------------

def ENDC () :
  return '\033[0m'

#---------------------------------------------------------------------------------------------------

def RED () :
  return '\033[91m'

#---------------------------------------------------------------------------------------------------

def BOLD () :
  return '\033[1m'

#---------------------------------------------------------------------------------------------------

def BOLD_RED () :
  return BOLD () + RED ()

#---------------------------------------------------------------------------------------------------

def asSeparator () :
  return "//" + ("-" * 78) + "\n"

#---------------------------------------------------------------------------------------------------

def cppSeparator () :
  return "//" + ("-" * 78) + "\n"

#---------------------------------------------------------------------------------------------------

def generateSVChandler ():
  sFile = asSeparator ()
  sFile += "//   SVC HANDLE (DOUBLE STACK MODE)\n"
  sFile += asSeparator ()
  sFile += "//\n"
  sFile += "//          PSP+32 -> |                            |\n"
  sFile += "//                    |----------------------------| -\n"
  sFile += "//          PSP+28 -> | xPSR                       |  |\n"
  sFile += "//                    |----------------------------|  |\n"
  sFile += "//          PSP+24 -> | PC (after SVC instruction) |  |\n"
  sFile += "//                    |----------------------------|  |\n"
  sFile += "//          PSP+20 -> | LR                         |  |\n"
  sFile += "//                    |----------------------------|  |\n"
  sFile += "//          PSP+16 -> | R12                        |  |  Saved by interrupt response\n"
  sFile += "//                    |----------------------------|  |\n"
  sFile += "//          PSP+12 -> | R3                         |  |\n"
  sFile += "//                    |----------------------------|  |\n"
  sFile += "//          PSP+8  -> | R2                         |  |\n"
  sFile += "//                    |----------------------------|  |\n"
  sFile += "//          PSP+4  -> | R1                         |  |\n"
  sFile += "//                    |----------------------------|  |\n"
  sFile += "//     /--- PSP ----> | R0                         |  |\n"
  sFile += "//     |              |----------------------------| -\n"
  sFile += "//     |              |                            |\n"
  sFile += "//     |\n"
  sFile += "//     |                                          *---------------------*\n"
  sFile += "//     |                                          | LR return code      | +36 [ 9]\n"
  sFile += "//     |                                          *---------------------*\n"
  sFile += "//     \----------------------------------------- | R13 (PSP)           | +32 [ 8]\n"
  sFile += "//                                                *---------------------*\n"
  sFile += "//                                                | R11                 | +28 [ 7]\n"
  sFile += "//                                                *---------------------*\n"
  sFile += "//                                                | R10                 | +24 [ 6]\n"
  sFile += "//                                                *---------------------*\n"
  sFile += "//                                                | R9                  | +20 [ 5]\n"
  sFile += "//                                                *---------------------*\n"
  sFile += "//                                                | R8                  | +16 [ 4]\n"
  sFile += "//                                                *---------------------*\n"
  sFile += "//                                                | R7                  | +12 [ 3]\n"
  sFile += "//                                                *---------------------*\n"
  sFile += "//                                                | R6                  | + 8 [ 2]\n"
  sFile += "//                                                *---------------------*\n"
  sFile += "//                                                | R5                  | + 4 [ 1]\n"
  sFile += "//  *------------------------------------*        *---------------------*\n"
  sFile += "//  | var.running.task.control.block.ptr +------> | R4                  | + 0 [ 0]\n"
  sFile += "//  *------------------------------------*        *---------------------*\n"
  sFile += "//\n"
  sFile += asSeparator () + "\n"
  sFile += "	.section	.bss.var.background.task.context, \"aw\", %nobits\n"
  sFile += "  .align	  2\n\n"
  sFile += "var.background.task.context:\n"
  sFile += "  .space  4\n\n"
  sFile += asSeparator () + "\n"
  sFile += "	.section	.text.interrupt.SVC, \"ax\", %progbits\n\n"
  sFile += "  .global interrupt.SVC\n"
  sFile += "  .type interrupt.SVC, %function\n\n"
  sFile += "interrupt.SVC:\n"
  sFile += "//----------------------------------------- Save preserved registers\n"
  sFile += "  push  {r4, lr}\n"
  sFile += "//----------------------------------------- R4 <- thread SP\n"
  sFile += "  mrs   r4, psp\n"
  sFile += "//----------------------------------------- R0 <- Address of instruction following SVC\n"
  sFile += "  ldr   r0, [r4, #24]   // 24 : 6 stacked registers before saved PC\n"
  sFile += "//----------------------------------------- R0 <- bits 0-7 of SVC instruction\n"
  sFile += "  subs  r0, #2         // R0 <- Address of SVC instruction\n"
  sFile += "  ldrb  r0, [r0]       // R0 is service call index\n"
  sFile += "//----------------------------------------- R1 <- address of dispatcher table\n"
  sFile += "  ldr   r1, =svc.dispatcher.table\n"
  sFile += "//----------------------------------------- R12 <- address of routine to call\n"
  sFile += "  lsls  r0, r0, #2 // R0 = (R0 << 2)\n"
  sFile += "  add   r1, r0     //\n"
  sFile += "  ldr   r1, [r1]   //\n"
  sFile += "  mov   r12, r1\n"
  sFile += "//----------------------------------------- Restore R0, R1, R2 and R3 from saved stack\n"
  sFile += "  ldmia r4!, {r0, r1, r2, r3} // R4 incremented by 16\n"
  sFile += "//----------------------------------------- R4 <- calling task context\n"
  sFile += "  ldr   r4, =var.running.task.control.block.ptr\n"
  sFile += "  ldr   r4, [r4]\n"
  sFile += "//----------------------------------------- Call service routine\n"
  sFile += "  blx   r12         // R4:calling task context address\n"
  sFile += "//--- Continues in sequence to handle.context.switch\n\n"
  sFile += asSeparator ()
  sFile += "//\n"
  sFile += "//  HANDLE CONTEXT SWITCH (DOUBLE STACK MODE)\n"
  sFile += "//\n"
  sFile += "//  On entry:\n"
  sFile += "//    - R4 contains the runnning task save context address,\n"
  sFile += "//    - R4 and LR of running task have been pushed on handler stack.\n"
  sFile += "//\n"
  sFile += asSeparator () + "\n"
  sFile += "handle.context.switch:\n"
  sFile += "//----------------------------------------- Select task to run\n"
  sFile += "  bl    kernel.select.task.to.run\n"
  sFile += "//----------------------------------------- R0 <- calling task context, R1 <- new task context\n"
  sFile += "  ldr   r1, =var.running.task.control.block.ptr\n"
  sFile += "  movs  r0, r4\n"
  sFile += "  ldr   r1, [r1]\n"
  sFile += "//----------------------------------------- Restore preserved registers\n"
  sFile += "  pop   {r2, r3}\n"
  sFile += "  movs  r4, r2\n"
  sFile += "  mov   lr, r3\n"
  sFile += "//----------------------------------------- Running task did change ?\n"
  sFile += "  cmp   r0, r1  // R0:calling task context, R1:new task context\n"
  sFile += "  bne   running.state.did.change\n"
  sFile += "  bx    lr  // No change\n"
  sFile += "//----------------------------------------- Save context of preempted task\n"
  sFile += "running.state.did.change:\n"
  sFile += "  mrs   r12, psp\n"
  sFile += "  cmp   r0, #0\n"
  sFile += "  beq   save.background.task.context\n"
  sFile += "//--- Save registers r4 to r11, PSP (stored in R12), LR\n"
  sFile += "  stm   r0!, {r4, r5, r6, r7}\n"
  sFile += "  mov   r4, r8\n"
  sFile += "  mov   r5, r9\n"
  sFile += "  mov   r6, r10\n"
  sFile += "  mov   r7, r11\n"
  sFile += "  stm   r0!, {r4, r5, r6, r7}\n"
  sFile += "  mov   r4, r12\n"
  sFile += "  mov   r5, lr\n"
  sFile += "  stm   r0!, {r4, r5}\n"
  sFile += "  b     perform.restore.context\n"
  sFile += "save.background.task.context:\n"
  sFile += "  ldr   r2, =var.background.task.context\n"
  sFile += "  mov   r3, r12\n"
  sFile += "  str   r3, [r2]\n"
  sFile += "//----------------------------------------- Restore context of activated task\n"
  sFile += "perform.restore.context:\n"
  sFile += "  cmp   r1, #0\n"
  sFile += "  beq   restore.background.task.context\n"
  sFile += "  ldm   r1!, {r4, r5, r6, r7}\n"
  sFile += "  ldm   r1!, {r2, r3}\n"
  sFile += "  mov   r8, r2\n"
  sFile += "  mov   r9, r3\n"
  sFile += "  ldm   r1!, {r2, r3}\n"
  sFile += "  mov   r10, r2\n"
  sFile += "  mov   r11, r3\n"
  sFile += "  ldm   r1!, {r2, r3}\n"
  sFile += "  mov   lr, r3\n"
  sFile += "  msr   psp, r2\n"
  sFile += "  bx    lr\n"
  sFile += "//----------------------------------------- Restore background task context\n"
  sFile += "restore.background.task.context:\n"
  sFile += "  ldr   r2, =var.background.task.context\n"
  sFile += "  ldr   r2, [r2]\n"
  sFile += "  msr   psp, r2\n"
  sFile += "  bx    lr\n\n"
  return sFile

#---------------------------------------------------------------------------------------------------

def generateSoftwareInterruptandler () :
  sFile = asSeparator ()
  sFile += "//\n"
  sFile += "//     SECTIONS: Software Interrupt Handler (two stack mode)\n"
  sFile += "//\n"
  sFile += asSeparator ()
  sFile += "//\n"
  sFile += "//                    |                            |\n"
  sFile += "//          PSP+32 -> |----------------------------| -\n"
  sFile += "//                    | xPSR                       |  |\n"
  sFile += "//          PSP+28 -> |----------------------------|  |\n"
  sFile += "//                    | PC                         |  |\n"
  sFile += "//          PSP+24 -> |----------------------------|  |\n"
  sFile += "//                    | LR                         |  |\n"
  sFile += "//          PSP+20 -> |----------------------------|  |\n"
  sFile += "//                    | R12                        |  |  Saved by interrupt response\n"
  sFile += "//          PSP+16 -> |----------------------------|  |\n"
  sFile += "//                    | R3                         |  |\n"
  sFile += "//          PSP+12 -> |----------------------------|  |\n"
  sFile += "//                    | R2                         |  |\n"
  sFile += "//          PSP+8  -> |----------------------------|  |\n"
  sFile += "//                    | R1                         |  |\n"
  sFile += "//          PSP+4  -> |----------------------------|  |\n"
  sFile += "//                    | R0                         |  |\n"
  sFile += "//          PSP    -> |----------------------------| -\n"
  sFile += "//\n"
  sFile += asSeparator () + "\n"
  sFile += "  .section  .text.interrupt.SWINT, \"ax\", %progbits\n\n"
  sFile += "  .global interrupt.SWINT\n"
  sFile += "  .type interrupt.SWINT, %function\n\n"
  sFile += "interrupt.SWINT:\n"
  sFile += "//--------------------- Save preserved registers\n"
  sFile += "  push  {r5, lr}\n"
  sFile += "//--------------------- R5 <- thread SP\n"
  sFile += "  mrs   r5, psp\n"
  sFile += "//--------------------- Restore R0, R1, R2 and R3 from saved stack\n"
  sFile += "  ldmia r5, {r0, r1, r2, r3}\n"
  sFile += "//--------------------- R12 <- Address section routine\n"
  sFile += "  ldr   r12, [r5, #16]     // 16 : 4 stacked registers before saved R12\n"
  sFile += "//--------------------- Call section routine\n"
  sFile += "  blx   r12\n"
  sFile += "//--------------------- Set return code (from R0 to R3) in stacked registers\n"
  sFile += "  stmia r5!, {r0, r1, r2, r3}    // R5 is thread SP\n"
  sFile += "//--------------------- Set R12 stacked register to 0\n"
  sFile += "  mov   r0, #0\n"
  sFile += "  str   r0, [r5]\n"
  sFile += "//--------------------- Restore preserved registers, return from interrupt\n"
  sFile += "  pop   {r5, pc}\n\n"
  sFile += asSeparator () + "\n"
  sFile += "  .section  .text.direct.call.or.call.software.interrupt, \"ax\", %progbits\n\n"
  sFile += "  .type direct.call.or.call.software.interrupt, %function\n\n"
  sFile += "direct.call.or.call.software.interrupt: // R12 contains the address of section implementation function\n"
  sFile += "//--------------------- Save preserved registers\n"
  sFile += "  push {r6, r7}\n"
  sFile += "  mrs  r6, IPSR          // IPSR[8...0] ≠ 0 in handler mode, = 0 in thread mode\n"
  sFile += "  mov  r7, #511\n"
  sFile += "  tst  r6, r7\n"
  sFile += "  bne  direct.call\n"
  sFile += "//--------------------- Software interrupt\n"
  sFile += "  ldr  r6, = 0xE000EF00  // Address of STIR control register\n"
  sFile += "  movs r7, # (80 - 16)   // Software Interrupt has number #80\n"
  sFile += "  str  r7, [r6]          // Generate Software Interrupt\n"
  sFile += "//--------------------- Wait for the exception is carried out\n"
  sFile += "wait.software.interrupt.done: // R12 is reset by interrupt handler\n"
  sFile += "  cmp  r12, #0\n"
  sFile += "  bne  wait.software.interrupt.done\n"
  sFile += "//--------------------- Restore preserved registers\n"
  sFile += "  pop  {r6, r7}\n"
  sFile += "  bx   lr\n"
  sFile += "//--------------------- Direct call\n"
  sFile += "direct.call:\n"
  sFile += "  pop  {r6, r7}\n"
  sFile += "  bx   r12              // in handler mode, call implementation routine directly\n\n"
  return sFile

#---------------------------------------------------------------------------------------------------

def generateSoftwareInterruptSection (sectionName, idx):
  sFile  = asSeparator () + "\n"
  sFile += "  .section .text." + sectionName + ", \"ax\", %progbits\n"
  sFile += "  .global " + sectionName +"\n"
  sFile += "  .align 1\n"
  sFile += "  .type " + sectionName +", %function\n\n"
  sFile += sectionName +":\n"
  sFile += "  .fnstart\n"
  sFile += "  ldr  r12, =section." + sectionName + "\n"
  sFile += "  b    direct.call.or.call.software.interrupt\n\n"
  sFile += ".Lfunc_end_" + sectionName +":\n"
  sFile += "  .size " + sectionName +", .Lfunc_end_" + sectionName +" - " + sectionName +"\n"
  sFile += "  .cantunwind\n"
  sFile += "  .fnend\n\n"
  return sFile

#---------------------------------------------------------------------------------------------------

def generateDisableInterruptSection (sectionName):
  sFile  = asSeparator ()
  sFile += "//   SECTION - " + sectionName + "\n"
  sFile += asSeparator () + "\n"
  sFile += "  .section .text." + sectionName + ", \"ax\", %progbits\n"
  sFile += "  .global " + sectionName +"\n"
  sFile += "  .align 1\n"
  sFile += "  .type " + sectionName +", %function\n\n"
  sFile += sectionName +":\n"
  sFile += "  .fnstart\n"
  sFile += "//--- Save preserved registers\n"
  sFile += "  push  {r6, lr}\n"
  sFile += "//--- Save interrupt enabled state\n"
  sFile += "  mrs   r6, PRIMASK\n"
  sFile += "//--- Disable interrupt\n"
  sFile += "  cpsid i\n"
  sFile += "//--- Call section, interrupts disabled\n"
  sFile += "  bl    section." + sectionName + "\n"
  sFile += "//--- Restore interrupt state\n"
  sFile += "  msr   PRIMASK, r6\n"
  sFile += "//--- Restore preserved registers and return\n"
  sFile += "  pop   {r6, pc}\n\n"
  sFile += ".Lfunc_end_" + sectionName +":\n"
  sFile += "  .size " + sectionName +", .Lfunc_end_" + sectionName +" - " + sectionName +"\n"
  sFile += "  .cantunwind\n"
  sFile += "  .fnend\n\n"
  return sFile

#---------------------------------------------------------------------------------------------------

def generateDisableInterruptAndSpinLockSection (sectionName):
  sFile  = asSeparator ()
  sFile += "//   SECTION - " + sectionName + "\n"
  sFile += asSeparator () + "\n"
  sFile += "  .section .text." + sectionName + ", \"ax\", %progbits\n"
  sFile += "  .global " + sectionName +"\n"
  sFile += "  .align 1\n"
  sFile += "  .type " + sectionName +", %function\n\n"
  sFile += sectionName +":\n"
  sFile += "  .fnstart\n"
  sFile += "//--- Save preserved registers\n"
  sFile += "  push  {r3, r4, r5, r6, r7, lr}\n"
  sFile += "//--- Save interrupt enabled state\n"
  sFile += "  mrs   r6, PRIMASK\n"
  sFile += "//--- Disable interrupt\n"
  sFile += "  cpsid i\n"
  sFile += "//--- R4 <- Address of CPUID control register (rp2040 datasheet, 2.3.1.7, page 42)\n"
  sFile += "  ldr   r4, = 0xD0000000 + 0\n"
  sFile += "//--- R5 <- Value of CPUID control register\n"
  sFile += "  ldr   r5, [r4] // R5 <- 0 for CPU 0, 1 for CPU 1\n"
  sFile += "//--- R7 <- Address for spinlock.acquired 16-bit variable (uint8_t [2])\n"
  sFile += "  ldr   r7, = spinlock.acquired\n"
  sFile += "//--- R7 <- Address for spinlock.acquired 8-bit variable for current cpu\n"
  sFile += "  adds  r7, r5\n"
  sFile += "//--- R5 <- Value of spinlock.acquired 8-bit variable for current cpu\n"
  sFile += "  ldrb r5, [r7]\n"
  sFile += "//--- R5 <- 1 if Already acquired by this CPU, 0 otrherwise\n"
  sFile += "  cmp  r5, #0\n"
  sFile += "//--- If already locked, call section directly\n"
  sFile += "  beq   " + sectionName +".acquire.spinlock\n"
  sFile += "//--- Call section, interrupts disabled\n"
  sFile += "  bl    section." + sectionName + "\n"
  sFile += "  b     " + sectionName +".exit\n"
  sFile += sectionName +".acquire.spinlock:\n"
  sFile += "//--- Set spinlock.acquired 8-bit variable for current cpu\n"
  sFile += "  movs  r5, #1\n"
  sFile += "  strb  r5, [r7]\n"
  sFile += "//--- R4 <- Address of SPINLOCK 0 (rp2040 datasheet, 2.3.1.7, page 42)\n"
  sFile += "  ldr   r4, = 0xD0000000 + 0x100\n"
  sFile += "//--- Read: attempt to claim the lock. Read value is nonzero if the lock was\n"
  sFile += "//    successfully claimed, or zero if the lock had already been claimed\n"
  sFile += "//    by a previous read (rp2040 datasheet, section 2.3.1.3 page 30).\n"
  sFile += sectionName +".spinlock.busy.wait:\n"
  sFile += "  ldr   r5, [r4]\n"
  sFile += "  cmp   r5, #0\n"
  sFile += "  beq   " + sectionName +".spinlock.busy.wait\n"
  sFile += "@  dmb\n"
  sFile += "//--- Call section, interrupts disabled, spinlock successfully claimed\n"
  sFile += "  bl    section." + sectionName + "\n"
  sFile += "//--- Write (any value): release the lock (rp2040 datasheet, section 2.3.1.3 page 30).\n"
  sFile += "//    The next attempt to claim the lock will be successful.\n"
  sFile += "  str   r5, [r4]\n"
  sFile += "//--- Clear spinlock.acquired 8-bit variable for current cpu\n"
  sFile += "  movs  r5, # 0\n"
  sFile += "  strb  r5, [r7]\n"
  sFile += "@  dmb\n"
  sFile += "//----------- Exit\n"
  sFile += sectionName +".exit:\n"
  sFile += "//--- Restore interrupt state\n"
  sFile += "  msr   PRIMASK, r6\n"
  sFile += "//--- Restore preserved registers and return\n"
  sFile += "  pop   {r3, r4, r5, r6, r7, pc}\n\n"
  sFile += ".Lfunc_end_" + sectionName +":\n"
  sFile += "  .size " + sectionName +", .Lfunc_end_" + sectionName +" - " + sectionName +"\n"
  sFile += "  .cantunwind\n"
  sFile += "  .fnend\n\n"
  return sFile

#---------------------------------------------------------------------------------------------------
#    ENTRY POINT
#---------------------------------------------------------------------------------------------------

#------------------------------ Interrupt dictionary
interruptDictionary = interrupt_names_raspberry_pi_pico.interruptNames ()
# print "Dest " + destinationFile
#------------------------------ Assembly destination file
destinationCppFile = sys.argv [1]
# print "Dest " + destinationAssemblerFile
#------------------------------ Assembly destination file
destinationAssemblerFile = sys.argv [2]
# print "Dest " + destinationAssemblerFile
#------------------------------ Service scheme
serviceScheme = sys.argv [3]
#------------------------------ Section scheme
sectionScheme = sys.argv [4]
#------------------------------ Header files
headerFiles = []
for i in range (5, len (sys.argv)):
  headerFiles.append (sys.argv [i])
#print headerFiles
#------------------------------ Destination file string
cppFile = ""
sFile  = "  .syntax unified\n"
sFile += "  .cpu cortex-m0plus\n"
sFile += "  .thumb\n\n"
#------------------------------ Explore header files
interruptServiceList = []
interruptSectionList = []
boolServiceSet = set ()
serviceList = []
sectionList = []
for header in headerFiles:
  with open (header) as f:
    for line in f:
      splitStr = line.strip ().split ("//$interrupt-section ")
      if len (splitStr) == 2 :
        interruptName = splitStr [1].strip ()
        if interruptName in interruptDictionary :
          interruptSectionList.append (interruptName)
          del interruptDictionary [interruptName]
        else:
          print (BOLD_RED () + "Error, interrupt '" + interruptName + "' does not exist, or is already assigned." + ENDC ())
          sys.exit (1)
      splitStr = line.strip ().split ("//$interrupt-service ")
      if len (splitStr) == 2 :
        interruptName = splitStr [1].strip ()
        if interruptName in interruptDictionary :
          interruptServiceList.append (interruptName)
          del interruptDictionary [interruptName]
        else:
          print (BOLD_RED () + "Error, interrupt '" + interruptName + "' does not exist, or is already assigned." + ENDC ())
          sys.exit (1)
      splitStr = line.strip ().split ("//$service ")
      if len (splitStr) == 2 :
        serviceName = splitStr [1].strip ()
        serviceList.append (serviceName)
      splitStr = line.strip ().split ("//$bool-service ")
      if len (splitStr) == 2 :
        serviceName = splitStr [1].strip ()
        serviceList.append (serviceName)
        boolServiceSet.add (serviceName)
      splitStr = line.strip ().split ("//$section ")
      if len (splitStr) == 2 :
        sectionName = splitStr [1].strip ()
        sectionList.append (sectionName)
#------------------------------ Has service ?
if (len (serviceList) > 0) and (serviceScheme == "") :
  print (BOLD_RED ()
         + "As the project defines service(s), the makefile.json file should have a \"SERVICE-SCHEME\" "
         + "entry (asoociated value: \"svc\")"
         +  ENDC ())
  sys.exit (1)
#------------------------------ Has sections ?
if (len (sectionList) > 0) and (sectionScheme == "") :
  print (BOLD_RED ()
         + "As the project defines section(s), the makefile.json file should have a \"SECTION-SCHEME\" "
         + "entry (possible associated value: \"disableInterrupt\", \"bkpt\", \"swint\")"
         +  ENDC ())
  sys.exit (1)
#------------------------------ Services
if serviceScheme == "svc" :
  sFile += generateSVChandler ()
  del interruptDictionary ["SVC"]
  sFile += asSeparator ()
  sFile += "//   SERVICES\n"
  idx = 1
  for service in serviceList :
    sFile += asSeparator () + "\n"
    sFile += "  .section .text." + service + ", \"ax\", %progbits\n"
    sFile += "  .global " + service +"\n"
    sFile += "  .align 1\n"
    sFile += "  .type " + service +", %function\n\n"
    sFile += service +":\n"
    sFile += "  .fnstart\n"
    sFile += "  svc #" + str (idx) + "\n"
    if service in boolServiceSet :
      sFile += "  ldr r3, = get.user.result\n"
      sFile += "  bx  r3\n\n"
    else:
      sFile += "  bx  lr\n\n"
    sFile += ".Lfunc_end_" + service +":\n"
    sFile += "  .size " + service +", .Lfunc_end_" + service +" - " + service +"\n"
    sFile += "  .cantunwind\n"
    sFile += "  .fnend\n\n"
    idx += 1
  sFile += asSeparator ()
  sFile += "//    SERVICE DISPATCHER TABLE\n"
  sFile += asSeparator () + "\n"
  sFile += "  .align   2\n"
  sFile += "  .global  svc.dispatcher.table\n\n"
  sFile += "svc.dispatcher.table:\n"
  sFile += "  .word cpu.0.phase3.init // 0\n"
  idx = 1
  for service in serviceList :
    sFile += "  .word service." + service + " // " + str (idx) + "\n"
    idx += 1
  sFile += "\n"
#------------------------------ Sections
if sectionScheme == "disableInterrupt" :
  for section in sectionList :
    sFile += generateDisableInterruptSection (section)
elif sectionScheme == "disableInterrupt-spinlock" :
  for section in sectionList :
    sFile += generateDisableInterruptAndSpinLockSection (section)
elif len (sectionList) > 0 :
  print (BOLD_RED ()
    + "In the makefile.json file, the \"SECTION-SCHEME\" key has an invalid \"" + sectionScheme + "\" value; "
    + "(possible value: \"disableInterrupt\", \"disableInterrupt-spinlock\")"
    +  ENDC ())
  sys.exit (1)
#------------------------------ Interrupts as service
for interruptServiceName in interruptServiceList :
  sFile += asSeparator ()
  sFile += "//   INTERRUPT - SERVICE: " + interruptServiceName + "\n"
  sFile += asSeparator () + "\n"
  sFile += "  .section .text.interrupt." + interruptServiceName + ", \"ax\", %progbits\n\n"
  sFile += "  .align  1\n"
  sFile += "  .global interrupt." + interruptServiceName + "\n"
  sFile += "  .type interrupt." + interruptServiceName + ", %function\n\n"
  sFile += "interrupt." + interruptServiceName + ":\n"
  sFile += "//----------------------------------------- Save preserved registers\n"
  sFile += "  push  {r4, lr}\n"
  sFile += "//----------------------------------------- Activity led On\n"
  sFile += "  ldr   r0, = 0xD0000000 + 0x014 // Address of GPIO_OUT_SET control register\n"
  sFile += "  ldr   r1, = (1 << 26) // Port GP26\n"
  sFile += "  str   r1, [r0]        // Turn on\n"
  sFile += "//----------------------------------------- R4 <- running task context\n"
  sFile += "  ldr   r4, =var.running.task.control.block.ptr\n"
  sFile += "  ldr   r4, [r4]\n"
  sFile += "//----------------------------------------- Call Interrupt handler\n"
  sFile += "  bl    interrupt.service." + interruptServiceName + "\n"
  sFile += "//----------------------------------------- Perform the context switch, if needed\n"
  sFile += "  b     handle.context.switch\n\n"
#------------------------------ Interrupts as section
for interruptSectionName in interruptSectionList :
  sFile += asSeparator ()
  sFile += "//   INTERRUPT - SECTION: " + interruptSectionName + "\n"
  sFile += asSeparator () + "\n"
  sFile += "  .section .text.interrupt." + interruptSectionName + ", \"ax\", %progbits\n\n"
  sFile += "  .align  1\n"
  sFile += "  .global interrupt." + interruptSectionName + "\n"
  sFile += "  .type interrupt." + interruptSectionName + ", %function\n\n"
  sFile += "interrupt." + interruptSectionName + ":\n"
  sFile += "//----------------------------------------- Activity led On\n"
  sFile += "  ldr   r0, =0xD0000000 + 0x014  // Address of SIO_GPIO_OUT_SET control register\n"
  sFile += "  ldr   r1, = (1 << 26)   // Port GP26 is ACTIVITY 0\n"
  sFile += "  str   r1, [r0]         // turn on\n"
  sFile += "//----------------------------------------- Goto interrupt function\n"
  sFile += "  ldr   r2, = interrupt.section." + interruptSectionName + "\n"
  sFile += "  bx    r2\n\n"
#------------------------------ Unused interrupts
for unusedInterruptName in interruptDictionary.keys () :
  sFile += asSeparator ()
  sFile += "//   INTERRUPT - UNUSED: " + unusedInterruptName + "\n"
  sFile += asSeparator () + "\n"
  sFile += "  .section .text.interrupt." + unusedInterruptName + ", \"ax\", %progbits\n\n"
  sFile += "  .align  1\n"
  sFile += "  .type interrupt." + unusedInterruptName + ", %function\n"
  sFile += "  .global interrupt." + unusedInterruptName + "\n\n"
  sFile += "interrupt." + unusedInterruptName + ":\n"
  sFile += "  movs r0, #" + str (interruptDictionary [unusedInterruptName]) + "\n"
  sFile += "//----------------------------------------- Goto unused interrupt function\n"
  sFile += "  ldr  r2, = unused.interrupt\n"
  sFile += "  bx   r2\n\n"
#------------------------------ Write destination file
sFile += asSeparator ()
f = open (destinationAssemblerFile, "wt")
f.write (sFile)
f.close()
f = open (destinationCppFile, "wt")
f.write (cppFile)
f.close()

#---------------------------------------------------------------------------------------------------
