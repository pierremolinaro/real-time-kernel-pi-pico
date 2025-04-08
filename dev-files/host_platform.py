#! /usr/bin/env python3
# -*- coding: UTF-8 -*-

#---------------------------------------------------------------------------------------------------

import os, sys

#---------------------------------------------------------------------------------------------------
#   FOR PRINTING IN COLOR
#---------------------------------------------------------------------------------------------------

class bcolors:
    HEADER = '\033[35m'
    BLUE = '\033[34m'
    GREEN = '\033[32m'
    WARNING = '\033[33m'
    FAIL = '\033[31m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    BOLD_BLUE = '\033[1m' + '\033[34m'
    BOLD_GREEN = '\033[1m' + '\033[32m'
    BOLD_RED = '\033[1m' + '\033[31m'

#---------------------------------------------------------------------------------------------------
#   PLATFORM
#---------------------------------------------------------------------------------------------------

def getPlatform () :
#--- Determine platform
  PLATFORM = ""
  (SYSTEM_NAME, MODE_NAME, RELEASE, VERSION, MACHINE) = os.uname ()
  #print "SYSTEM_NAME '" + SYSTEM_NAME + "'"
  #print "MACHINE '" + MACHINE + "'"
#   if (MACHINE == "i386") & (SYSTEM_NAME == "Darwin") :
#     PLATFORM = "mac"
#   elif (MACHINE == "x86_64") & (SYSTEM_NAME == "Darwin") :
#     PLATFORM = "mac"
  if (MACHINE == "arm64") & (SYSTEM_NAME == "Darwin") :
    PLATFORM = "mac"
#   elif (MACHINE == "x86_64") & (SYSTEM_NAME == "Linux") :
#     PLATFORM = "linux"
#   elif (MACHINE == "i386") & (SYSTEM_NAME == "Linux") :
#     PLATFORM = "linux32"
#   elif (MACHINE == "i686") & (SYSTEM_NAME == "Linux") :
#     PLATFORM = "linux32"
  else:
    print (bcolors.BOLD_RED + "*** Unknown platform (SYSTEM_NAME = \"" + SYSTEM_NAME + "\", MACHINE = \"" + MACHINE + "\") ***" + bcolors.ENDC)
    sys.exit (1) ;
  return PLATFORM

#---------------------------------------------------------------------------------------------------

