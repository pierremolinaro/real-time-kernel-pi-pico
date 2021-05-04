#! /usr/bin/env python
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

import makefile
import download_and_install_elf_to_uf2
import code_builder

#---------------------------------------------------------------------------------------------------
#    ENTRY POINT
#---------------------------------------------------------------------------------------------------

def buildDeployment (PRODUCT, verbose):
  goal = PRODUCT + ".uf2"
  rule = makefile.Rule ([PRODUCT + ".uf2"], "Converting elf to UF2 " + PRODUCT + ".elf")
  rule.mDependences.append (PRODUCT + ".elf")
  rule.mDependences.append ("makefile.json")
  ELF2UF2_TOOL_PATH = download_and_install_elf_to_uf2.compile_install_elf2uf2 ()
  rule.mCommand.append (ELF2UF2_TOOL_PATH)
  if verbose :
    rule.mCommand.append ("-v")
  rule.mCommand.append (PRODUCT + ".elf")
  rule.mCommand.append (PRODUCT + ".uf2")
  return (goal, rule)

#---------------------------------------------------------------------------------------------------

def performDeployment (DEPLOYMENT_HELPER_DIR, PRODUCT) :
  DEPLOY_COMMAND = [
    DEPLOYMENT_HELPER_DIR + "/uf2conv.py",
    "-d", "/dev/cu.usbmodem14301",
    "--deploy", PRODUCT + ".uf2"
  ]
  print (makefile.BOLD_BLUE () + "Flashing Raspberry Pi Pico..." + makefile.ENDC ())
  code_builder.runProcess (DEPLOY_COMMAND)
  print (makefile.BOLD_GREEN () + "Success" + makefile.ENDC ())

#---------------------------------------------------------------------------------------------------
