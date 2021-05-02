#! /usr/bin/env python
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

import sys, os, interrupt_names_raspberry_pi_pico

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
  sCode = asSeparator ()
  sCode += "//   SVC HANDLE (DOUBLE STACK MODE)\n"
  sCode += asSeparator ()
  sCode += "//\n"
  sCode += "//          PSP+32 -> |                            |\n"
  sCode += "//                    |----------------------------| -\n"
  sCode += "//          PSP+28 -> | xPSR                       |  |\n"
  sCode += "//                    |----------------------------|  |\n"
  sCode += "//          PSP+24 -> | PC (after SVC instruction) |  |\n"
  sCode += "//                    |----------------------------|  |\n"
  sCode += "//          PSP+20 -> | LR                         |  |\n"
  sCode += "//                    |----------------------------|  |\n"
  sCode += "//          PSP+16 -> | R12                        |  |  Saved by interrupt response\n"
  sCode += "//                    |----------------------------|  |\n"
  sCode += "//          PSP+12 -> | R3                         |  |\n"
  sCode += "//                    |----------------------------|  |\n"
  sCode += "//          PSP+8  -> | R2                         |  |\n"
  sCode += "//                    |----------------------------|  |\n"
  sCode += "//          PSP+4  -> | R1                         |  |\n"
  sCode += "//                    |----------------------------|  |\n"
  sCode += "//     /--- PSP ----> | R0                         |  |\n"
  sCode += "//     |              |----------------------------| -\n"
  sCode += "//     |              |                            |\n"
  sCode += "//     |\n"
  sCode += "//     |                                          *---------------------*\n"
  sCode += "//     |                                          | LR return code      | +36 [ 9]\n"
  sCode += "//     |                                          *---------------------*\n"
  sCode += "//     \----------------------------------------- | R13 (PSP)           | +32 [ 8]\n"
  sCode += "//                                                *---------------------*\n"
  sCode += "//                                                | R11                 | +28 [ 7]\n"
  sCode += "//                                                *---------------------*\n"
  sCode += "//                                                | R10                 | +24 [ 6]\n"
  sCode += "//                                                *---------------------*\n"
  sCode += "//                                                | R9                  | +20 [ 5]\n"
  sCode += "//                                                *---------------------*\n"
  sCode += "//                                                | R8                  | +16 [ 4]\n"
  sCode += "//                                                *---------------------*\n"
  sCode += "//                                                | R7                  | +12 [ 3]\n"
  sCode += "//                                                *---------------------*\n"
  sCode += "//                                                | R6                  | + 8 [ 2]\n"
  sCode += "//                                                *---------------------*\n"
  sCode += "//                                                | R5                  | + 4 [ 1]\n"
  sCode += "//  *------------------------------------*        *---------------------*\n"
  sCode += "//  | var.running.task.control.block.ptr +------> | R4                  | + 0 [ 0]\n"
  sCode += "//  *------------------------------------*        *---------------------*\n"
  sCode += "//\n"
  sCode += asSeparator () + "\n"
  sCode += "	.section	.bss.var.background.task.context, \"aw\", %nobits\n"
  sCode += "  .align	  2\n\n"
  sCode += "var.background.task.context:\n"
  sCode += "  .space  4\n\n"
  sCode += asSeparator () + "\n"
  sCode += "	.section	.text.interrupt.SVC, \"ax\", %progbits\n\n"
  sCode += "  .global interrupt.SVC\n"
  sCode += "  .type interrupt.SVC, %function\n\n"
  sCode += "interrupt.SVC:\n"
  sCode += "//----------------------------------------- Save preserved registers\n"
  sCode += "  push  {r4, lr}\n"
  sCode += "//----------------------------------------- R4 <- thread SP\n"
  sCode += "  mrs   r4, psp\n"
  sCode += "//----------------------------------------- R0 <- Address of instruction following SVC\n"
  sCode += "  ldr   r0, [r4, #24]   // 24 : 6 stacked registers before saved PC\n"
  sCode += "//----------------------------------------- R0 <- bits 0-7 of SVC instruction\n"
  sCode += "  subs  r0, #2         // R0 <- Address of SVC instruction\n"
  sCode += "  ldrb  r0, [r0]       // R0 is service call index\n"
  sCode += "//----------------------------------------- R1 <- address of dispatcher table\n"
  sCode += "  ldr   r1, =svc.dispatcher.table\n"
  sCode += "//----------------------------------------- R12 <- address of routine to call\n"
  sCode += "  lsls  r0, r0, #2 // R0 = (R0 << 2)\n"
  sCode += "  add   r1, r0     //\n"
  sCode += "  ldr   r1, [r1]   //\n"
  sCode += "  mov   r12, r1\n"
  sCode += "//----------------------------------------- Restore R0, R1, R2 and R3 from saved stack\n"
  sCode += "  ldmia r4!, {r0, r1, r2, r3} // R4 incremented by 16\n"
  sCode += "//----------------------------------------- R4 <- calling task context\n"
  sCode += "  ldr   r4, =var.running.task.control.block.ptr\n"
  sCode += "  ldr   r4, [r4]\n"
  sCode += "//----------------------------------------- Call service routine\n"
  sCode += "  blx   r12         // R4:calling task context address\n"
  sCode += "//--- Continues in sequence to handle.context.switch\n\n"
  sCode += asSeparator ()
  sCode += "//\n"
  sCode += "//  HANDLE CONTEXT SWITCH (DOUBLE STACK MODE)\n"
  sCode += "//\n"
  sCode += "//  On entry:\n"
  sCode += "//    - R4 contains the runnning task save context address,\n"
  sCode += "//    - R4 and LR of running task have been pushed on handler stack.\n"
  sCode += "//\n"
  sCode += asSeparator () + "\n"
  sCode += "handle.context.switch:\n"
  sCode += "//----------------------------------------- Select task to run\n"
  sCode += "  bl    kernel.select.task.to.run\n"
  sCode += "//----------------------------------------- R0 <- calling task context, R1 <- new task context\n"
  sCode += "  ldr   r1, =var.running.task.control.block.ptr\n"
  sCode += "  movs  r0, r4\n"
  sCode += "  ldr   r1, [r1]\n"
  sCode += "//----------------------------------------- Restore preserved registers\n"
  sCode += "  pop   {r2, r3}\n"
  sCode += "  movs  r4, r2\n"
  sCode += "  mov   lr, r3\n"
  sCode += "//----------------------------------------- Running task did change ?\n"
  sCode += "  cmp   r0, r1  // R0:calling task context, R1:new task context\n"
  sCode += "  bne   running.state.did.change\n"
  sCode += "  bx    lr  // No change\n"
  sCode += "//----------------------------------------- Save context of preempted task\n"
  sCode += "running.state.did.change:\n"
  sCode += "  mrs   r12, psp\n"
  sCode += "  cmp   r0, #0\n"
  sCode += "  beq   save.background.task.context\n"
  sCode += "//--- Save registers r4 to r11, PSP (stored in R12), LR\n"
  sCode += "  stm   r0!, {r4, r5, r6, r7}\n"
  sCode += "  mov   r4, r8\n"
  sCode += "  mov   r5, r9\n"
  sCode += "  mov   r6, r10\n"
  sCode += "  mov   r7, r11\n"
  sCode += "  stm   r0!, {r4, r5, r6, r7}\n"
  sCode += "  mov   r4, r12\n"
  sCode += "  mov   r5, lr\n"
  sCode += "  stm   r0!, {r4, r5}\n"
  sCode += "  b     perform.restore.context\n"
  sCode += "save.background.task.context:\n"
  sCode += "  ldr   r2, =var.background.task.context\n"
  sCode += "  mov   r3, r12\n"
  sCode += "  str   r3, [r2]\n"
  sCode += "//----------------------------------------- Restore context of activated task\n"
  sCode += "perform.restore.context:\n"
  sCode += "  cmp   r1, #0\n"
  sCode += "  beq   restore.background.task.context\n"
  sCode += "  ldm   r1!, {r4, r5, r6, r7}\n"
  sCode += "  ldm   r1!, {r2, r3}\n"
  sCode += "  mov   r8, r2\n"
  sCode += "  mov   r9, r3\n"
  sCode += "  ldm   r1!, {r2, r3}\n"
  sCode += "  mov   r10, r2\n"
  sCode += "  mov   r11, r3\n"
  sCode += "  ldm   r1!, {r2, r3}\n"
  sCode += "  mov   lr, r3\n"
  sCode += "  msr   psp, r2\n"
  sCode += "  bx    lr\n"
  sCode += "//----------------------------------------- Restore background task context\n"
  sCode += "restore.background.task.context:\n"
  sCode += "  ldr   r2, =var.background.task.context\n"
  sCode += "  ldr   r2, [r2]\n"
  sCode += "  msr   psp, r2\n"
  sCode += "  bx    lr\n\n"
  return sCode

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
requiredSectionScheme = sys.argv [4]
#------------------------------ Header files
headerFiles = []
for i in range (5, len (sys.argv)):
  headerFiles.append (sys.argv [i])
#print headerFiles
#------------------------------ Destination file string
cppFile = "#include \"all-headers.h\"\n"
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
#------------------------------ Find all section schemes
DEV_FILES_DIR = os.path.dirname (os.path.realpath (__file__))
SECTION_SCHEME_DIR = DEV_FILES_DIR + "/generators-section"
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
