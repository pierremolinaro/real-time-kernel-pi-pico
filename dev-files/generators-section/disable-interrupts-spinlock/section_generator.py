#! /usr/bin/env python
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

def asSeparator () :
  return "//" + ("-" * 78) + "\n"

#---------------------------------------------------------------------------------------------------

def generateDisableInterruptAndSpinLockHeader () :
  sCode  = asSeparator ()
  sCode += "//   SECTION VARIABLES\n\n"
  sCode += "	.section	.bss.spinlock.acquired, \"aw\", %nobits\n\n"
  sCode += "  .global	  spinlock.acquired\n\n"
  sCode += "spinlock.acquired: // C type: uint8_t [2]\n"
  sCode += "  .space	2\n\n"
  return ("", sCode)

#---------------------------------------------------------------------------------------------------

def generateDisableInterruptAndSpinLockSection (sectionName):
  sCode  = asSeparator ()
  sCode += "//   SECTION - " + sectionName + "\n"
  sCode += asSeparator () + "\n"
  sCode += "  .section .text." + sectionName + ", \"ax\", %progbits\n"
  sCode += "  .global " + sectionName +"\n"
  sCode += "  .align 1\n"
  sCode += "  .type " + sectionName +", %function\n\n"
  sCode += sectionName +":\n"
  sCode += "  .fnstart\n"
  sCode += "//--- Save preserved registers\n"
  sCode += "  push  {r4, r5, r6, r7, lr}\n"
  sCode += "//--- Save interrupt enabled state\n"
  sCode += "  mrs   r6, PRIMASK\n"
  sCode += "//--- Disable interrupt\n"
  sCode += "  cpsid i\n"
  sCode += "//--- R4 <- Address of CPUID control register (rp2040 datasheet, 2.3.1.7, page 42)\n"
  sCode += "  ldr   r4, = 0xD0000000 + 0\n"
  sCode += "//--- R5 <- Value of CPUID control register\n"
  sCode += "  ldr   r5, [r4] // R5 <- 0 for CPU 0, 1 for CPU 1\n"
  sCode += "//--- R7 <- Address for spinlock.acquired 16-bit variable (uint8_t [2])\n"
  sCode += "  ldr   r7, = spinlock.acquired\n"
  sCode += "//--- R7 <- Address for spinlock.acquired 8-bit variable for current cpu\n"
  sCode += "  adds  r7, r5\n"
  sCode += "//--- R5 <- Value of spinlock.acquired 8-bit variable for current cpu\n"
  sCode += "  ldrb r5, [r7]\n"
  sCode += "//--- R5 <- 1 if Already acquired by this CPU, 0 otherwise\n"
  sCode += "  cmp  r5, #0\n"
  sCode += "//--- If already locked, call section directly\n"
  sCode += "  beq   " + sectionName +".acquire.spinlock\n"
  sCode += "//--- Call section, interrupts disabled\n"
  sCode += "  bl    section." + sectionName + "\n"
  sCode += "  b     " + sectionName +".exit\n"
  sCode += sectionName +".acquire.spinlock:\n"
  sCode += "//--- Set spinlock.acquired 8-bit variable for current cpu\n"
  sCode += "  movs  r5, #1\n"
  sCode += "  strb  r5, [r7]\n"
  sCode += "//--- R4 <- Address of SPINLOCK 0 (rp2040 datasheet, 2.3.1.7, page 42)\n"
  sCode += "  ldr   r4, = 0xD0000000 + 0x100\n"
  sCode += "//--- Read: attempt to claim the lock. Read value is nonzero if the lock was\n"
  sCode += "//    successfully claimed, or zero if the lock had already been claimed\n"
  sCode += "//    by a previous read (rp2040 datasheet, section 2.3.1.3 page 30).\n"
  sCode += sectionName +".spinlock.busy.wait:\n"
  sCode += "  ldr   r5, [r4]\n"
  sCode += "  cmp   r5, #0\n"
  sCode += "  beq   " + sectionName +".spinlock.busy.wait\n"
  sCode += "@  dmb\n"
  sCode += "//--- Call section, interrupts disabled, spinlock successfully claimed\n"
  sCode += "  bl    section." + sectionName + "\n"
  sCode += "//--- Write (any value): release the lock (rp2040 datasheet, section 2.3.1.3 page 30).\n"
  sCode += "//    The next attempt to claim the lock will be successful.\n"
  sCode += "  str   r5, [r4]\n"
  sCode += "//--- Clear spinlock.acquired 8-bit variable for current cpu\n"
  sCode += "  movs  r5, # 0\n"
  sCode += "  strb  r5, [r7]\n"
  sCode += "@  dmb\n"
  sCode += "//----------- Exit\n"
  sCode += sectionName +".exit:\n"
  sCode += "//--- Restore interrupt state\n"
  sCode += "  msr   PRIMASK, r6\n"
  sCode += "//--- Restore preserved registers and return\n"
  sCode += "  pop   {r4, r5, r6, r7, pc}\n\n"
  sCode += ".Lfunc_end_" + sectionName +":\n"
  sCode += "  .size " + sectionName +", .Lfunc_end_" + sectionName +" - " + sectionName +"\n"
  sCode += "  .cantunwind\n"
  sCode += "  .fnend\n\n"
  return ("", sCode)

#---------------------------------------------------------------------------------------------------
#    ENTRY POINT
#---------------------------------------------------------------------------------------------------

def buildInterruptHandlerCode (sectionList):
  #------------------------------ Destination file strings
  cppFile = ""
  sFile   = ""
  #------------------------------ Header
  (cppCode, sCode) = generateDisableInterruptAndSpinLockHeader ()
  cppFile += cppCode
  sFile += sCode
  #------------------------------ Iterate over sectionList
  for section in sectionList :
    (cppCode, sCode) = generateDisableInterruptAndSpinLockSection (section)
    cppFile += cppCode
    sFile += sCode
  #------------------------------ Return
  return (cppFile, sFile)

#---------------------------------------------------------------------------------------------------
