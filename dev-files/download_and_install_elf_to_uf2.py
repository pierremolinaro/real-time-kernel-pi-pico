#! /usr/bin/env python
# -*- coding: UTF-8 -*-

#---------------------------------------------------------------------------------------------------

import os, subprocess, sys
import urllib
#---------------------------------------------------------------------------------------------------

import archive_directory
import tool_directory
import dev_platform

#---------------------------------------------------------------------------------------------------
#   FOR PRINTING IN COLOR
#---------------------------------------------------------------------------------------------------

class bcolors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    BOLD_BLUE = '\033[1m' + '\033[94m'
    BOLD_GREEN = '\033[1m' + '\033[92m'
    BOLD_RED = '\033[1m' + '\033[91m'

#---------------------------------------------------------------------------------------------------
#   runCommand
#---------------------------------------------------------------------------------------------------

def runCommand (cmd) :
  str = "+"
  for s in cmd:
    str += " " + s
  print bcolors.BOLD_BLUE + str + bcolors.ENDC
  returncode = subprocess.call (cmd)
  if returncode != 0 :
    sys.exit (returncode)

#---------------------------------------------------------------------------------------------------
#   ARCHIVE DOWNLOAD
#---------------------------------------------------------------------------------------------------

def downloadArchive (archiveURL, archivePath):
  print ("URL: "+ archiveURL)
  print "Downloading..."
  runCommand (["curl", "--fail", "-L", archiveURL, "-o", archivePath])

#---------------------------------------------------------------------------------------------------
#  Install GCC
#---------------------------------------------------------------------------------------------------

def install_elf2uf2 (TOOL_DIR, TOOL_NAME) :
  print bcolors.BOLD_GREEN + "Installing elf2uf2..." + bcolors.ENDC
  #------------------------------------------------------------------ Archive dir
  COMPILER_ARCHIVE_DIR = archive_directory.createAndGetArchiveDirectory ()
  PICO_SDK = "pico-sdk-master"
  #------------------------------------------------------------------ Source File
  SOURCE_FILE = COMPILER_ARCHIVE_DIR + "/elf-to-uf2/main.cpp"
  #------------------------------------------------------------------ Download
  if not os.path.exists (COMPILER_ARCHIVE_DIR + "/" + PICO_SDK + ".zip"):
    url = "https://github.com/raspberrypi/pico-sdk/archive/master.zip"
    runCommand (["rm", "-f", COMPILER_ARCHIVE_DIR + "/" + PICO_SDK + ".zip.downloading"])
    downloadArchive (url, COMPILER_ARCHIVE_DIR + "/" + PICO_SDK + ".zip.downloading")
    runCommand (["mv",
                 COMPILER_ARCHIVE_DIR + "/" + PICO_SDK + ".zip.downloading",
                 COMPILER_ARCHIVE_DIR + "/" + PICO_SDK + ".zip"
                ])
  #------------------------------------------------------------------ Unzip
  if not os.path.exists (COMPILER_ARCHIVE_DIR + "/" + PICO_SDK):
    savedCurrentDir = os.getcwd ()
    os.chdir (COMPILER_ARCHIVE_DIR)
    runCommand (["unzip", "-q", COMPILER_ARCHIVE_DIR + "/" + PICO_SDK + ".zip"])
    os.chdir (savedCurrentDir)
  #------------------------------------------------------------------ Install
  TOOL_PATH = TOOL_DIR + "/" + TOOL_NAME
  if not os.path.exists (TOOL_DIR):
    os.mkdir (TOOL_DIR)
  SOURCE_FILE = COMPILER_ARCHIVE_DIR + "/" + PICO_SDK + "/tools/elf2uf2/main.cpp"
  INCLUDE_DIR = COMPILER_ARCHIVE_DIR + "/" + PICO_SDK + "/src/common/boot_uf2/include"
  runCommand (["g++", "-O2", "-std=c++11", "-I", INCLUDE_DIR, SOURCE_FILE, "-o", TOOL_PATH])

#---------------------------------------------------------------------------------------------------
#   GET GCC TOOL DIRECTORY
#---------------------------------------------------------------------------------------------------

def compile_install_elf2uf2 () :
#--- Absolute path of tool directory
  TOOL_DIR = tool_directory.toolDirectory () + "/bin"
  TOOL_PATH = TOOL_DIR + "/elf2uf2"
#--- Install tool ?
  if not os.path.exists (TOOL_PATH) :
    install_elf2uf2 (TOOL_DIR, "elf2uf2")
#--- Return tool directory
  return TOOL_PATH

#---------------------------------------------------------------------------------------------------

