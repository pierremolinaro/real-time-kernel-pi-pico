#! /usr/bin/env python
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

import sys, os, shutil, json

#---------------------------------------------------------------------------------------------------

#----------------------------------------------------------------- Get script absolute path
scriptDir = os.path.dirname (os.path.abspath (sys.argv [0]))
os.chdir (scriptDir)
#----------------------------------------------------------------- Import common definitions
jsonFilePath = os.path.abspath (scriptDir + "/makefile.json")
f = open (jsonFilePath, "r")
dictionary = json.loads (f.read ())
f.close ()
devDir = os.path.abspath (dictionary ["DEV-DIR"])
sys.path.append (devDir)
import common_definitions
#-----------------------------------------------------------------
GENERATED_SOURCE_DIR = scriptDir + "/" + common_definitions.generatedSourceDirectory ()
if os.path.exists (GENERATED_SOURCE_DIR):
  shutil.rmtree (GENERATED_SOURCE_DIR)
BUILD_DIR = scriptDir + "/" + common_definitions.buildDirectory ()
if os.path.exists (BUILD_DIR):
  shutil.rmtree (BUILD_DIR)
PRODUCT_DIR = scriptDir + "/" + common_definitions.productDirectory ()
if os.path.exists (PRODUCT_DIR):
  shutil.rmtree (PRODUCT_DIR)
AS_DIR = scriptDir + "/" + common_definitions.asDirectory ()
if os.path.exists (AS_DIR):
  shutil.rmtree (AS_DIR)
for file in os.listdir (scriptDir) :
  fullPath = scriptDir + "/" + file
  if os.path.isfile (fullPath) and file.startswith ("2-run-") and file.endswith (".py") :
    os.remove (fullPath)
#---
print ("Cleaning " + os.path.basename (scriptDir) + " done")

#---------------------------------------------------------------------------------------------------
