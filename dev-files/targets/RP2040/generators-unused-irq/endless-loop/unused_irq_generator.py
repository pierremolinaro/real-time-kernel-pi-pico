#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

def asSeparator () :
  return "//" + ("-" * 78) + "\n"

#---------------------------------------------------------------------------------------------------

def generateUnusedInterrupt (unusedInterruptName, unusedInterruptNumber) :
  sCode  = asSeparator ()
  sCode += "//   INTERRUPT - UNUSED: " + unusedInterruptName + "\n"
  sCode += asSeparator () + "\n"
  sCode += "  .section .text.interrupt." + unusedInterruptName + ", \"ax\", %progbits\n\n"
  sCode += "  .align  1\n"
  sCode += "  .type interrupt." + unusedInterruptName + ", %function\n"
  sCode += "  .global interrupt." + unusedInterruptName + "\n\n"
  sCode += "interrupt." + unusedInterruptName + ":\n"
  sCode += "  b    interrupt." + unusedInterruptName + "\n\n"
  return ("", sCode)

#---------------------------------------------------------------------------------------------------
#    ENTRY POINT
#---------------------------------------------------------------------------------------------------

def buildUnusedInterruptCode (unusedInterruptDictionary):
  #------------------------------ Destination file strings
  cppFile = ""
  sFile   = ""
  #------------------------------ Iterate on unused interrupt list
  for unusedInterruptName in unusedInterruptDictionary.keys () :
    unusedInterruptNumber = unusedInterruptDictionary [unusedInterruptName]
    (cppCode, sCode) = generateUnusedInterrupt (unusedInterruptName, unusedInterruptNumber)
    cppFile += cppCode
    sFile += sCode
  #------------------------------ Return
  return (cppFile, sFile)

#---------------------------------------------------------------------------------------------------
