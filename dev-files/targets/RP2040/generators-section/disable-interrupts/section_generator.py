#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

def asSeparator () :
  return "//" + ("-" * 78) + "\n"

#---------------------------------------------------------------------------------------------------

def generateDisableInterruptSection (sectionName):
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
  sCode += "  push  {r6, lr}\n"
  sCode += "//--- Save interrupt enabled state\n"
  sCode += "  mrs   r6, PRIMASK\n"
  sCode += "//--- Disable interrupt\n"
  sCode += "  cpsid i\n"
  sCode += "//--- Call section, interrupts disabled\n"
  sCode += "  bl    section." + sectionName + "\n"
  sCode += "//--- Restore interrupt state\n"
  sCode += "  msr   PRIMASK, r6\n"
  sCode += "//--- Restore preserved registers and return\n"
  sCode += "  pop   {r6, pc}\n\n"
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
  #------------------------------ Iterate on section sectionList
  for section in sectionList :
    (cppCode, sCode) = generateDisableInterruptSection (section)
    cppFile += cppCode
    sFile += sCode
  #------------------------------ Return
  return (cppFile, sFile)

#---------------------------------------------------------------------------------------------------
