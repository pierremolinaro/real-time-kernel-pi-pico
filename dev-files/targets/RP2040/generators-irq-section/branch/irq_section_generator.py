#! /usr/bin/env python
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

def asSeparator () :
  return "//" + ("-" * 78) + "\n"

#---------------------------------------------------------------------------------------------------

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
  sCode += "  ldr   r0, =0xD0000000 + 0x014  // Address of SIO_GPIO_OUT_SET control register\n"
  sCode += "  ldr   r1, = (1 << 26)   // Port GP26 is ACTIVITY 0\n"
  sCode += "  str   r1, [r0]         // turn on\n"
  sCode += "//----------------------------------------- Goto interrupt function\n"
  sCode += "  ldr   r2, = interrupt.section." + interruptSectionName + "\n"
  sCode += "  bx    r2\n\n"
  return ("", sCode)

#---------------------------------------------------------------------------------------------------
#    ENTRY POINT
#---------------------------------------------------------------------------------------------------

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

#---------------------------------------------------------------------------------------------------
