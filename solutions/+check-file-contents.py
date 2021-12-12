#! /usr/bin/env python3
# -*- coding: UTF-8 -*-

#---------------------------------------------------------------------------------------------------

import subprocess
import sys
import os

#---------------------------------------------------------------------------------------------------
#   FOR PRINTING IN COLOR
#---------------------------------------------------------------------------------------------------

def RED () :
  return '\033[91m'

#---------------------------------------------------------------------------------------------------

def GREEN () :
  return '\033[92m'

#---------------------------------------------------------------------------------------------------

def YELLOW () :
  return '\033[93m'

#---------------------------------------------------------------------------------------------------

def ENDC () :
  return '\033[0m'

#---------------------------------------------------------------------------------------------------

def BOLD () :
  return '\033[1m'

#---------------------------------------------------------------------------------------------------

def BOLD_GREEN () :
  return BOLD () + GREEN ()

#---------------------------------------------------------------------------------------------------

def BOLD_RED () :
  return BOLD () + RED ()

#---------------------------------------------------------------------------------------------------

#--- Get script absolute path
scriptDir = os.path.dirname (os.path.abspath (sys.argv [0]))
#--- Enumerate directories
fileDictionary = {}
contentDictionary = {}
for stepDir in sorted (os.listdir (scriptDir)) :
   sourceDir = os.path.join (scriptDir, stepDir, "sources")
   if os.path.isdir (sourceDir) :
     for file in sorted (os.listdir (sourceDir)) :
       if not file.startswith ('.') :
         fileAbsPath = os.path.join (sourceDir, file)
         fileRelPath = os.path.join (stepDir, "sources", file)
         f = open (fileAbsPath, 'rt')
         contents = f.read ()
         f.close ()
         if contents.find ('\t') >= 0 :
           print (BOLD_RED () + "File '" + fileRelPath + "' contains htab" + ENDC ())
#           sys.exit (1)
         if not file in fileDictionary :
           fileDictionary [file] = (contents, fileAbsPath)
         else :
           (otherContents, otherPath) = fileDictionary [file]
           if otherContents != contents :
             print (BOLD_RED () + "Different contents for:")
             print (BOLD_RED () + "  '" + fileRelPath + "'")
             print (BOLD_RED () + "  '" + otherPath + "'" + ENDC ())
#             sys.exit (1)
         if not contents in contentDictionary :
           contentDictionary [contents] = fileRelPath
         else :
           otherPath = contentDictionary [contents]
           if os.path.basename (os.path.normpath (otherPath)) != os.path.basename (os.path.normpath (fileRelPath)) :
             print (BOLD_RED () + "Same contents for:")
             print (BOLD_RED () + "  '" + fileRelPath + "'")
             print (BOLD_RED () + "  '" + otherPath + "'" + ENDC ())
#             sys.exit (1)
print (BOLD_GREEN () + "OK" + ENDC ())

#---------------------------------------------------------------------------------------------------
