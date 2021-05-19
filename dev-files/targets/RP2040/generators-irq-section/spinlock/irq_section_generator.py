#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
#------------------------------------------------------------------------------

def asSeparator () :
  return "//" + ("-" * 78) + "\n"

#------------------------------------------------------------------------------

def generateInterruptSection (interruptSectionName) :
  sCode  = asSeparator ()
  sCode += "//   INTERRUPT - SECTION: " + interruptSectionName + "\n"
  sCode += asSeparator () + "\n"
  sCode += "  .section .text.interrupt." + interruptSectionName + ", \"ax\", %progbits\n\n"
  sCode += "  .align  1\n"
  sCode += "  .global interrupt." + interruptSectionName + "\n"
  sCode += "  .type interrupt." + interruptSectionName + ", %function\n\n"
  sCode += "interrupt." + interruptSectionName + ":\n"
  sCode += "//----------------------------------------- Activity led On\n"
  sCode += "  MACRO_ACTIVITY_LED_0_ON\n"
  sCode += "//--- Save registers\n"
  sCode += "  push  {r4, r5, lr}\n"
  sCode += "//--- R4 <- Address of SPINLOCK 0 (rp2040 datasheet, 2.3.1.7, page 42)\n"
  sCode += "  ldr   r4, = 0xD0000000 + 0x100\n"
  sCode += "//--- Read: attempt to claim the lock. Read value is nonzero if the lock was\n"
  sCode += "//    successfully claimed, or zero if the lock had already been claimed\n"
  sCode += "//    by a previous read (rp2040 datasheet, section 2.3.1.3 page 30).\n"
  sCode += interruptSectionName +".spinlock.busy.wait:\n"
  sCode += "  ldr   r5, [r4]\n"
  sCode += "  cmp   r5, #0\n"
  sCode += "  beq   " + interruptSectionName +".spinlock.busy.wait\n"
  sCode += "//--- Call section, interrupts disabled, spinlock successfully claimed\n"
  sCode += "  bl    interrupt.section." + interruptSectionName + "\n"
  sCode += "//--- Write (any value): release the lock (rp2040 datasheet, section 2.3.1.3 page 30).\n"
  sCode += "//    The next attempt to claim the lock will be successful.\n"
  sCode += "  str   r5, [r4]\n"
  sCode += "//--- Return\n"
  sCode += "  pop  {r4, r5, pc}\n\n"
  return ("", sCode)

#------------------------------------------------------------------------------
#    ENTRY POINT
#------------------------------------------------------------------------------

def buildSectionInterruptCode (interruptSectionList):
  #------------------------------ Destination file strings
  cppFile = ""
  sFile   = ""
  #------------------------------ Iterate on section sectionList
  for interruptSectionName in interruptSectionList :
    (cppCode, sCode) = generateInterruptSection (interruptSectionName)
    cppFile += cppCode
    sFile += sCode
  #------------------------------ Return
  return (cppFile, sFile)

#------------------------------------------------------------------------------
