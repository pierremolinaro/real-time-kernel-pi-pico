#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

import sys, os

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
#    ENTRY POINT
#---------------------------------------------------------------------------------------------------

#------------------------------ Target directory
targetDirectory = sys.argv [1]
#------------------------------ Assembly destination file
destinationCppFile = sys.argv [2]
# print "Dest " + destinationAssemblerFile
#------------------------------ Assembly destination file
destinationAssemblerFile = sys.argv [3]
# print "Dest " + destinationAssemblerFile
#------------------------------ Service scheme
requiredServiceScheme = sys.argv [4]
#------------------------------ Section scheme
requiredSectionScheme = sys.argv [5]
#------------------------------ IRQ Section scheme
requiredIRQSectionScheme = sys.argv [6]
#------------------------------ Unused Interrupt scheme
unusedInterruptScheme = sys.argv [7]
#------------------------------ Header files
headerFiles = []
for i in range (8, len (sys.argv)):
  headerFiles.append (sys.argv [i])
#print headerFiles
#------------------------------ Import interrupt_names
HELPER_DIR = targetDirectory + "/helpers"
sys.path.append (HELPER_DIR)
import interrupt_names
#------------------------------ Interrupt dictionary
interruptDictionary = interrupt_names.interruptNames ()
#------------------------------ Assembly file contents
assemblyHeaderFile = HELPER_DIR + "/assembly-header.s"
f = open (assemblyHeaderFile, "r")
sFile = f.read ()
f.close ()
#------------------------------ C++ file contents
cppFile = "#include \"all-headers.h\"\n"
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
#------------------------------ SERVICES -----------------------------------------------
# DEV_FILES_DIR = os.path.dirname (os.path.realpath (__file__))
#------------------------------ Find all section schemes
SERVICE_SCHEME_DIR = targetDirectory + "/generators-service"
# print ("SERVICE_SCHEME_DIR " + SERVICE_SCHEME_DIR)
allServiceSchemes = []
foundServiceScheme = False
for root, dirs, files in os.walk (SERVICE_SCHEME_DIR) :
  for name in dirs:
    # print ("DIR " + name)
    allServiceSchemes.append (name)
    if name == requiredServiceScheme :
      foundServiceScheme = True
#------------------------------ Has service ?
if (len (serviceList) > 0) and (requiredServiceScheme == "") :
  s = "As the project defines service(s), the makefile.json file should have a \"SERVICE-SCHEME\" "
  s += "entry, possible associated value:\n"
  for name in allServiceSchemes :
    s += "  - \"" + name + "\"\n"
  print (BOLD_RED () + s +  ENDC ())
  sys.exit (1)
elif (not foundServiceScheme) and (requiredServiceScheme != "") :
  s = "In the makefile.json file, the \"SERVICE-SCHEME\" entry "
  s += "value (\"" + requiredServiceScheme + "\") is not implemented; "
  s += "possible associated value:\n"
  for name in allServiceSchemes :
    s += "  - \"" + name + "\"\n"
  print (BOLD_RED () + s +  ENDC ())
  sys.exit (1)
#------------------------------ Generate service
if foundServiceScheme and (requiredServiceScheme != "") :
  SELECTED_SERVICE_SCHEME_DIR = SERVICE_SCHEME_DIR + "/" + requiredServiceScheme
  #   print ("SELECTED_SERVICE_SCHEME_DIR : " + SELECTED_SERVICE_SCHEME_DIR)
  sys.path.append (SELECTED_SERVICE_SCHEME_DIR)
  import service_generator
  (cppCode, sCode, interruptDictionary) = service_generator.buildServiceCode (serviceList, boolServiceSet, interruptServiceList, interruptDictionary)
  cppFile += cppCode
  sFile += sCode
#------------------------------ SECTION ------------------------------------------------
#------------------------------ Find all section schemes
SECTION_SCHEME_DIR = targetDirectory + "/generators-section"
# print ("SECTION_SCHEME_DIR " + SECTION_SCHEME_DIR)
allSectionSchemes = []
foundSectionScheme = False
for root, dirs, files in os.walk (SECTION_SCHEME_DIR) :
  for name in dirs:
    # print ("DIR " + name)
    allSectionSchemes.append (name)
    if name == requiredSectionScheme :
      foundSectionScheme = True
#------------------------------ Has sections ?
if (len (sectionList) > 0) and (requiredSectionScheme == "") :
  s = "As the project defines section(s), the makefile.json file should have a \"SECTION-SCHEME\" "
  s += "entry, possible associated value:\n"
  for name in allSectionSchemes :
    s += "  - \"" + name + "\"\n"
  print (BOLD_RED () + s +  ENDC ())
  sys.exit (1)
elif (not foundSectionScheme) and (requiredSectionScheme != "") :
  s = "In the makefile.json file, the \"SECTION-SCHEME\" entry "
  s += "value (\"" + requiredSectionScheme + "\") is not implemented; "
  s += "possible associated value:\n"
  for name in allSectionSchemes :
    s += "  - \"" + name + "\"\n"
  print (BOLD_RED () + s +  ENDC ())
  sys.exit (1)
#------------------------------ Generate sections
if foundSectionScheme and (requiredSectionScheme != "") :
  SELECTED_SECTION_SCHEME_DIR = SECTION_SCHEME_DIR + "/" + requiredSectionScheme
  #   print ("SELECTED_SECTION_SCHEME_DIR : " + SELECTED_SECTION_SCHEME_DIR)
  sys.path.append (SELECTED_SECTION_SCHEME_DIR)
  import section_generator
  (cppCode, sCode) = section_generator.buildInterruptHandlerCode (sectionList)
  cppFile += cppCode
  sFile += sCode
#------------------------------ INTERRUPTS AS SECTION ----------------------------------
#------------------------------ Find all irq section schemes
IRQ_SECTION_SCHEME_DIR = targetDirectory + "/generators-irq-section"
# print ("IRQ_SECTION_SCHEME_DIR " + IRQ_SECTION_SCHEME_DIR)
allIRQSectionSchemes = []
foundIRQSectionScheme = False
for root, dirs, files in os.walk (IRQ_SECTION_SCHEME_DIR) :
  for name in dirs:
    # print ("DIR " + name)
    allIRQSectionSchemes.append (name)
    if name == requiredIRQSectionScheme :
      foundIRQSectionScheme = True
#------------------------------ Has irq sections ?
if (len (interruptSectionList) > 0) and (requiredIRQSectionScheme == "") :
  s = "As the project defines section(s), the makefile.json file should have a \"SECTION-SCHEME\" "
  s += "entry, possible associated value:\n"
  for name in allIRQSectionSchemes :
    s += "  - \"" + name + "\"\n"
  print (BOLD_RED () + s +  ENDC ())
  sys.exit (1)
elif (not foundIRQSectionScheme) and (requiredIRQSectionScheme != "") :
  s = "In the makefile.json file, the \"SECTION-SCHEME\" entry "
  s += "value (\"" + requiredIRQSectionScheme + "\") is not implemented; "
  s += "possible associated value:\n"
  for name in allIRQSectionSchemes :
    s += "  - \"" + name + "\"\n"
  print (BOLD_RED () + s +  ENDC ())
  sys.exit (1)
#------------------------------ Generate irq sections
if foundIRQSectionScheme and (requiredIRQSectionScheme != "") :
  SELECTED_IRQ_SECTION_SCHEME_DIR = IRQ_SECTION_SCHEME_DIR + "/" + requiredIRQSectionScheme
  #   print ("SELECTED_IRQ_SECTION_SCHEME_DIR : " + SELECTED_IRQ_SECTION_SCHEME_DIR)
  sys.path.append (SELECTED_IRQ_SECTION_SCHEME_DIR)
  import irq_section_generator
  (cppCode, sCode) = irq_section_generator.buildSectionInterruptCode (interruptSectionList)
  cppFile += cppCode
  sFile += sCode
#------------------------------ UNUSED INTERRUPTS --------------------------------------
#------------------------------ Find all unused interrupt schemes
UNUSED_IRQ_SCHEME_DIR = targetDirectory + "/generators-unused-irq"
# print ("UNUSED_IRQ_SCHEME_DIR " + UNUSED_IRQ_SCHEME_DIR)
allUnusedInterruptSchemes = []
foundUnusedInterruptScheme = False
for root, dirs, files in os.walk (UNUSED_IRQ_SCHEME_DIR) :
  for name in dirs:
    # print ("DIR " + name)
    allUnusedInterruptSchemes.append (name)
    if name == unusedInterruptScheme :
      foundUnusedInterruptScheme = True
#------------------------------ Has unused interrupt(s) ?
if (interruptDictionary) and (unusedInterruptScheme == "") :
  s = "As the project defines unused interrupt(s), the makefile.json file should have a \"UNUSED-IRQ-SCHEME\" "
  s += "entry, possible associated value:\n"
  for name in allUnusedInterruptSchemes :
    s += "  - \"" + name + "\"\n"
  print (BOLD_RED () + s +  ENDC ())
  sys.exit (1)
elif (not foundUnusedInterruptScheme) and (unusedInterruptScheme != "") :
  s = "In the makefile.json file, the \"UNUSED-IRQ-SCHEME\" entry "
  s += "value (\"" + unusedInterruptScheme + "\") is not implemented; "
  s += "possible associated value:\n"
  for name in allUnusedInterruptSchemes :
    s += "  - \"" + name + "\"\n"
  print (BOLD_RED () + s +  ENDC ())
  sys.exit (1)
#------------------------------ Generate irq sections
if foundUnusedInterruptScheme and (unusedInterruptScheme != "") :
  SELECTED_UNUSED_IRQ_SCHEME_DIR = UNUSED_IRQ_SCHEME_DIR + "/" + unusedInterruptScheme
  #   print ("SELECTED_UNUSED_IRQ_SCHEME_DIR : " + SELECTED_UNUSED_IRQ_SCHEME_DIR)
  sys.path.append (SELECTED_UNUSED_IRQ_SCHEME_DIR)
  import unused_irq_generator
  (cppCode, sCode) = unused_irq_generator.buildUnusedInterruptCode (interruptDictionary)
  cppFile += cppCode
  sFile += sCode







#------------------------------ Unused interrupts
# for unusedInterruptName in interruptDictionary.keys () :
#   sFile += asSeparator ()
#   sFile += "//   INTERRUPT - UNUSED: " + unusedInterruptName + "\n"
#   sFile += asSeparator () + "\n"
#   sFile += "  .section .text.interrupt." + unusedInterruptName + ", \"ax\", %progbits\n\n"
#   sFile += "  .align  1\n"
#   sFile += "  .type interrupt." + unusedInterruptName + ", %function\n"
#   sFile += "  .global interrupt." + unusedInterruptName + "\n\n"
#   sFile += "interrupt." + unusedInterruptName + ":\n"
#   sFile += "  movs r0, #" + str (interruptDictionary [unusedInterruptName]) + "\n"
#   sFile += "//----------------------------------------- Goto unused interrupt function\n"
#   sFile += "  ldr  r2, = unused.interrupt\n"
#   sFile += "  bx   r2\n\n"
#------------------------------ Write destination file
sFile += asSeparator ()
f = open (destinationAssemblerFile, "wt")
f.write (sFile)
f.close()
f = open (destinationCppFile, "wt")
f.write (cppFile)
f.close()

#---------------------------------------------------------------------------------------------------
