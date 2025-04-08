#! /usr/bin/env python3
# -*- coding: UTF-8 -*-

#---------------------------------------------------------------------------------------------------

import os, subprocess, sys
import urllib
#---------------------------------------------------------------------------------------------------

import archive_directory
import tool_directory
import host_platform

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

#-----------------------------------------------------------------------------------------
#   DISTRIBUTION GCC, COMPILER REPOSITORY URL (version 12.2.MPACBTI-Rel1 du 22 mars 2023)
#-----------------------------------------------------------------------------------------

def gccarm () :
  url  = "https://developer.arm.com/-/media/Files/downloads/gnu/"
  url += "12.2.mpacbti-rel1/binrel/arm-gnu-toolchain-12.2.mpacbti-rel1-darwin-arm64-arm-none-eabi.tar.xz"
  directory = "arm-gnu-toolchain-12.2.mpacbti-rel1-darwin-arm64-arm-none-eabi"
  return (url, directory)

#-----------------------------------------------------------------------------------------
#   DISTRIBUTION GCC, COMPILER REPOSITORY URL (version 12.3 du 22 décembre 2022)
#-----------------------------------------------------------------------------------------

# def gccarm () :
#   url  = "https://developer.arm.com/-/media/Files/downloads/gnu/"
#   url += "12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-darwin-arm64-arm-none-eabi.tar.xz"
#   directory = "arm-gnu-toolchain-12.2.rel1-darwin-arm64-arm-none-eabi"
#   return (url, directory)

#-----------------------------------------------------------------------------------------
#   DISTRIBUTION GCC, COMPILER REPOSITORY URL (version 11.3.rel1 du 8 août 2022)
#-----------------------------------------------------------------------------------------

# def gccarm () :
#   url  = "https://developer.arm.com/-/media/Files/downloads/gnu/"
#   url += "12.2.mpacbti-bet1/binrel/arm-gnu-toolchain-12.2.mpacbti-bet1-darwin-x86_64-arm-none-eabi.tar.xz"
#   directory = "arm-gnu-toolchain-12.2.mpacbti-bet1-darwin-x86_64-arm-none-eabi"
#   return (url, directory)

#-----------------------------------------------------------------------------------------
#   DISTRIBUTION GCC, COMPILER REPOSITORY URL (version 11.3.rel1 du 8 août 2022)
#-----------------------------------------------------------------------------------------

# def gccarm () :
#   url  = "https://developer.arm.com/-/media/Files/downloads/"
#   url += "gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-darwin-x86_64-arm-none-eabi.tar.xz"
#   directory = "arm-gnu-toolchain-11.3.rel1-darwin-x86_64-arm-none-eabi"
#   return (url, directory)

#-----------------------------------------------------------------------------------------
#   DISTRIBUTION GCC, COMPILER REPOSITORY URL (version 10-2020-q4-major du 11 décembre 2020)
#-----------------------------------------------------------------------------------------

# def gccarm () :
#   url  = "https://developer.arm.com/-/media/Files/downloads/"
#   url += "gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-mac.tar.bz2"
#   directory = "gcc-arm-none-eabi-10-2020-q4-major"
#   return (url, directory)

#---------------------------------------------------------------------------------------------------
#   runCommand
#---------------------------------------------------------------------------------------------------

def runCommand (cmd) :
  str = "+"
  for s in cmd:
    str += " " + s
  print (bcolors.BOLD_BLUE + str + bcolors.ENDC)
  returncode = subprocess.call (cmd)
  if returncode != 0 :
    sys.exit (returncode)

#---------------------------------------------------------------------------------------------------
#   ARCHIVE DOWNLOAD
#---------------------------------------------------------------------------------------------------

def downloadArchive (archiveURL, archivePath):
  print ("URL: "+ archiveURL)
  print ("Downloading...")
  runCommand (["curl", "--fail", "-L", archiveURL, "-o", archivePath])

#---------------------------------------------------------------------------------------------------
#  Install GCC
#---------------------------------------------------------------------------------------------------

def install_gcc (INSTALL_DIR) :
  print (bcolors.BOLD_GREEN + "Install GCC tools..." + bcolors.ENDC)
  (url, DIRECTORY) = gccarm ()
  EXTENSION = url.split(".")[-1] # "xz", "bz2"
#   print ("URL " + url)
#   print ("DISTRIBUTION " + DIRECTORY)
#   print ("EXTENSION " + EXTENSION)
  #------------------------------------------------------------------ Archive dir
  COMPILER_ARCHIVE_DIR = archive_directory.createAndGetArchiveDirectory ()
  #------------------------------------------------------------------ Download
  if not os.path.exists (COMPILER_ARCHIVE_DIR + "/" + DIRECTORY + ".tar." + EXTENSION):
    runCommand (["rm", "-f", COMPILER_ARCHIVE_DIR + "/" + DIRECTORY + ".tar." + EXTENSION + ".downloading"])
    downloadArchive (url, COMPILER_ARCHIVE_DIR + "/" + DIRECTORY + ".tar." + EXTENSION + ".downloading")
    runCommand (["mv",
                 COMPILER_ARCHIVE_DIR + "/" + DIRECTORY + ".tar." + EXTENSION + ".downloading",
                 COMPILER_ARCHIVE_DIR + "/" + DIRECTORY + ".tar." + EXTENSION + ""
                ])
  #------------------------------------------------------------------ Install
  if not os.path.exists (INSTALL_DIR):
    os.mkdir (INSTALL_DIR)
  if not os.path.exists (INSTALL_DIR + "/" + DIRECTORY):
    runCommand (["cp", COMPILER_ARCHIVE_DIR + "/" + DIRECTORY + ".tar." + EXTENSION, INSTALL_DIR + "/" + DIRECTORY + ".tar." + EXTENSION])
    savedCurrentDir = os.getcwd ()
    os.chdir (INSTALL_DIR)
    print (bcolors.BOLD_BLUE + "+ cd " + INSTALL_DIR + bcolors.ENDC)
    runCommand (["tar", "xvf", DIRECTORY + ".tar." + EXTENSION])
    runCommand (["rm", DIRECTORY + ".tar." + EXTENSION])
    os.chdir (savedCurrentDir)


#---------------------------------------------------------------------------------------------------
#   GET GCC TOOL DIRECTORY
#---------------------------------------------------------------------------------------------------

# def installGCCARMandGetToolDirectory () :
# #--- Absolute path of tool directory
#   TOOL_DIRECTORY = tool_directory.toolDirectory () + "/" + distributionGCC ()
# #--- Install tool ?
#   if not os.path.exists (TOOL_DIRECTORY) :
#     install_gcc (tool_directory.toolDirectory ())
# #--- Return tool directory
#   return TOOL_DIRECTORY

#-----------------------------------------------------------------------------------------
#   GET GCC TOOL DIRECTORY
#-----------------------------------------------------------------------------------------

def installGCCARMandGetToolDirectory () :
#--- Absolute path of tool directory
  (url, directory) = gccarm ()
  TOOL_DIRECTORY = tool_directory.toolDirectory () + "/" + directory
#--- Install tool ?
  if not os.path.exists (TOOL_DIRECTORY) :
    install_gcc (tool_directory.toolDirectory ())
#--- Return tool directory
  return TOOL_DIRECTORY

#---------------------------------------------------------------------------------------------------

