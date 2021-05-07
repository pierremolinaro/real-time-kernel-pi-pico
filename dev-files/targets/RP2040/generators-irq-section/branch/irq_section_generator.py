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
  sCode += "//----------------------------------------- Goto interrupt function\n"
  sCode += "  ldr   r2, = interrupt.section." + interruptSectionName + "\n"
  sCode += "  bx    r2\n\n"
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
