#! /usr/bin/env python
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

import sys, os, subprocess, shutil, json

#---------------------------------------------------------------------------------------------------

import makefile
import common_definitions
import download_and_install_gccarm
import download_and_install_elf_to_uf2
import dev_platform

#---------------------------------------------------------------------------------------------------
#   Run process and wait for termination
#---------------------------------------------------------------------------------------------------

def runProcess (command) :
  returncode = subprocess.call (command)
  if returncode != 0 :
    print (makefile.BOLD_RED () + "Error " + str (returncode) + makefile.ENDC ())
    sys.exit (returncode)

#---------------------------------------------------------------------------------------------------
#   Run process, get output and wait for termination
#---------------------------------------------------------------------------------------------------

def runProcessAndGetOutput (command) :
  result = ""
  childProcess = subprocess.Popen (command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
  while True:
    out = childProcess.stdout.read(1)
    if out == '' and childProcess.poll() != None:
      break
    if out != '' :
      result += out
#--- Wait for subprocess termination
  if childProcess.poll () == None :
    childProcess.wait ()
  if childProcess.returncode != 0 :
    print (makefile.BOLD_RED () + "Error " + str (childProcess.returncode) + makefile.ENDC ())
    sys.exit (childProcess.returncode)
  return result

#---------------------------------------------------------------------------------------------------
#   dictionaryFromJsonFile                                                                                             *
#---------------------------------------------------------------------------------------------------

def dictionaryFromJsonFile (file) :
  result = {}
  if not os.path.exists (os.path.abspath (file)):
    print (makefile.BOLD_RED () + "The '" + file + "' file does not exist" + makefile.ENDC ())
    sys.exit (1)
  try:
    f = open (file, "r")
    result = json.loads (f.read ())
    f.close ()
  except:
    print (makefile.BOLD_RED () + "Syntax error in " + file + makefile.ENDC ())
    sys.exit (1)
  return result


#---------------------------------------------------------------------------------------------------
#   buildCode
#---------------------------------------------------------------------------------------------------

def buildCode (GOAL, projectDir, maxConcurrentJobs, showCommand):
#--------------------------------------------------------------------------- Prepare
  os.chdir (projectDir)
  make = makefile.Make (GOAL)
  make.mMacTextEditor = "BBEdit" # "Atom"
  allGoal = []
#--------------------------------------------------------------------------- Install Linux UDEV rules ?
  platform = dev_platform.getPlatform ()
  if (platform == "linux") or (platform == "linux32") :
    import udev_on_linux
    udev_on_linux.installUDEVrulesOnLinux ()
#--------------------------------------------------------------------------- Install compiler ?
  BASE_NAME = "arm-none-eabi"
  TOOL_DIR = download_and_install_gccarm.installGCCARMandGetToolDirectory ()
  AS_TOOL_WITH_OPTIONS = [TOOL_DIR + "/bin/" + BASE_NAME + "-as", "-mthumb", "-mcpu=cortex-m0plus"]
  COMPILER_TOOL_WITH_OPTIONS = [TOOL_DIR + "/bin/" + BASE_NAME + "-gcc", "-mthumb", "-mcpu=cortex-m0plus"]
  LD_TOOL_WITH_OPTIONS = [TOOL_DIR + "/bin/" + BASE_NAME + "-ld"]
  LD_TOOL_WITH_OPTIONS = COMPILER_TOOL_WITH_OPTIONS
  OBJCOPY_TOOL_WITH_OPTIONS = [TOOL_DIR + "/bin/" + BASE_NAME + "-objcopy"]
  DISPLAY_OBJ_SIZE_TOOL = [TOOL_DIR + "/bin/" + BASE_NAME + "-size"]
  OBJDUMP_TOOL = TOOL_DIR + "/bin/" + BASE_NAME + "-objdump"
#--------------------------------------------------------------------------- Install elf2uf2 ?
  ELF2UF2_TOOL_PATH = download_and_install_elf_to_uf2.compile_install_elf2uf2 ()
#--------------------------------------------------------------------------- Analyze JSON file
  print (makefile.BOLD_GREEN () + "--- Making " + projectDir + makefile.ENDC ())
  dictionaire = dictionaryFromJsonFile (projectDir + "/makefile.json")
#--- PLATFORM
  linkerScript = "raspberry-pi-pico.ld"
  platformName = "RASPBERRY_PI_PICO"
#--- ASSERTION_GENERATION
  ASSERTION_GENERATION = False
  if dictionaire.has_key ("ASSERTION-GENERATION") and dictionaire ["ASSERTION-GENERATION"] :
    ASSERTION_GENERATION = True
#--- CPU_MHZ
  CPU_MHZ = 0
  if dictionaire.has_key ("CPU-MHZ") :
    CPU_MHZ = dictionaire ["CPU-MHZ"]
#--- SOURCE_FILE_DIRECTORIES
  SOURCE_FILE_DIRECTORIES = []
  if dictionaire.has_key ("SOURCE-DIR") :
    SOURCE_FILE_DIRECTORIES = dictionaire ["SOURCE-DIR"]
#--- GROUP_SOURCES
  GROUP_SOURCES = False
  if dictionaire.has_key ("GROUP-SOURCES") :
    GROUP_SOURCES = dictionaire ["GROUP-SOURCES"]
#--- TASK_COUNT
  TASK_COUNT = "0" # Means TASK_COUNT is not defined by JSON file
  if dictionaire.has_key ("TASK-COUNT") :
    TASK_COUNT = str (dictionaire ["TASK-COUNT"])
#--- LTO
  usesLTO = False
  if dictionaire.has_key ("LTO") and dictionaire ["LTO"] :
    usesLTO = True
#--- SERVICE
  serviceScheme = ""
  if dictionaire.has_key ("SERVICE-SCHEME") :
    serviceScheme = dictionaire ["SERVICE-SCHEME"]
#--- SECTION
  sectionScheme = ""
  if dictionaire.has_key ("SECTION-SCHEME") :
    sectionScheme = dictionaire ["SECTION-SCHEME"]
#--------------------------------------------------------------------------- Directories
  BUILD_DIR = common_definitions.buildDirectory ()
  GENERATED_SOURCE_DIR = common_definitions.generatedSourceDirectory ()
  PRODUCT_DIR = common_definitions.productDirectory ()
  ASBUILD_DIR = common_definitions.asDirectory ()
#--------------------------------------------------------------------------- Build source lists
  includeDirsInCompilerCommand = ["-I", GENERATED_SOURCE_DIR, "-I", projectDir]
  H_SOURCE_LIST = []
  CPP_SOURCE_LIST = []
  S_SOURCE_LIST = []
  for f in SOURCE_FILE_DIRECTORIES :
    for root, dirs, files in os.walk (f) :
      includeDirsInCompilerCommand += ["-I", root]
      for name in files:
        sourcePath = os.path.join (root, name)
        (b, extension) = os.path.splitext (sourcePath)
        if extension == ".cpp" :
          CPP_SOURCE_LIST.append (sourcePath)
        elif extension == ".h" :
          H_SOURCE_LIST.append (sourcePath)
        elif extension == ".s" :
          S_SOURCE_LIST.append (sourcePath)
        elif extension != "" : # Ceci permet d'ignorer les fichés cachés (dont les noms commencent par un point)
          print (makefile.MAGENTA () + makefile.BOLD () + "Note: unhandled file " + sourcePath + makefile.ENDC ())
#--------------------------------------------------------------------------- Build base header file
  baseHeader_file = GENERATED_SOURCE_DIR + "/base.h"
  H_SOURCE_LIST.insert (0, baseHeader_file)
  rule = makefile.Rule ([baseHeader_file], "Build base header file")
  rule.mOpenSourceOnError = False
  rule.mDependences.append ("makefile.json")
  rule.mCommand += ["../../dev-files/build_base_header_file.py", baseHeader_file, str (CPU_MHZ), TASK_COUNT, platformName, "1" if ASSERTION_GENERATION else "0"]
  rule.mPriority = -1
  make.addRule (rule)
#--------------------------------------------------------------------------- Build all header file
  allHeadersSecondaryDependenceFile = BUILD_DIR + "/all-headers.dep"
  allHeaders_file = GENERATED_SOURCE_DIR + "/all-headers.h"
  rule = makefile.Rule ([allHeaders_file, allHeadersSecondaryDependenceFile], "Build all headers file")
  rule.mOpenSourceOnError = False
  rule.mDependences.append ("makefile.json")
  rule.mDependences += H_SOURCE_LIST
  rule.mCommand += ["../../dev-files/build_all_header_file.py", allHeaders_file, allHeadersSecondaryDependenceFile]
  rule.mCommand += H_SOURCE_LIST
  rule.enterSecondaryDependanceFile (allHeadersSecondaryDependenceFile, make)
  rule.mPriority = -1
  make.addRule (rule)
#--------------------------------------------------------------------------- Build interrupt handler files
  interruptHandlerSFile = GENERATED_SOURCE_DIR + "/interrupt-handlers.s"
  interruptHandlerCppFile = GENERATED_SOURCE_DIR + "/interrupt-handler-helper.cpp"
  S_SOURCE_LIST.append (interruptHandlerSFile)
  CPP_SOURCE_LIST.append (interruptHandlerCppFile)
  rule = makefile.Rule ([interruptHandlerSFile, interruptHandlerCppFile], "Build interrupt files")
  rule.mOpenSourceOnError = False
  rule.mDependences += H_SOURCE_LIST
  rule.mDependences.append ("makefile.json")
  rule.mDependences.append ("../../dev-files/build_interrupt_handlers.py")
  rule.mCommand += ["../../dev-files/build_interrupt_handlers.py"]
  rule.mCommand += [interruptHandlerCppFile]
  rule.mCommand += [interruptHandlerSFile]
  rule.mCommand += [serviceScheme]
  rule.mCommand += [sectionScheme]
  rule.mCommand += H_SOURCE_LIST
  rule.mPriority = -1
  make.addRule (rule)
#--------------------------------------------------------------------------- Group sources ?
  if GROUP_SOURCES :
    allSourceFile = GENERATED_SOURCE_DIR + "/all-sources.cpp"
    rule = makefile.Rule ([allSourceFile], "Group all sources")
    rule.mOpenSourceOnError = False
    rule.mDependences += CPP_SOURCE_LIST
    rule.mDependences.append ("makefile.json")
    rule.mCommand += ["../../dev-files/build_grouped_sources.py", allSourceFile]
    rule.mCommand += CPP_SOURCE_LIST
    rule.mPriority = -1
    make.addRule (rule)
    CPP_SOURCE_LIST = [allSourceFile]
#--------------------------------------------------------------------------- Build makefile rules
  objectFileList = []
  asObjectFileList = []
#--- CPP source files
  for sourcePath in CPP_SOURCE_LIST :
    source = os.path.basename (sourcePath)
    objectFile = BUILD_DIR + "/" + source + ".o"
    objectFileForChecking = BUILD_DIR + "/" + source + ".check.o"
    asObjectFile = BUILD_DIR + "/" + source + ".s"
  #--- Checking source
    rule = makefile.Rule ([objectFileForChecking], "Checking " + source)
    rule.mOpenSourceOnError = False
    rule.mDependences.append (allHeaders_file)
    rule.mDependences.append (sourcePath)
    rule.mDependences.append ("makefile.json")
    rule.enterSecondaryDependanceFile (objectFileForChecking + ".dep", make)
    rule.mCommand += COMPILER_TOOL_WITH_OPTIONS
    rule.mCommand += common_definitions.checkModeOptions ()
    rule.mCommand += common_definitions.C_Cpp_optimizationOptions ()
    rule.mCommand += common_definitions.Cpp_actualOptions (False)
    rule.mCommand += ["-c", sourcePath]
    rule.mCommand += ["-o", objectFileForChecking]
    rule.mCommand += ["-DSTATIC="]
    rule.mCommand += includeDirsInCompilerCommand
    rule.mCommand += ["-MD", "-MP", "-MF", objectFileForChecking + ".dep"]
    make.addRule (rule)
    rule.mPriority = -1
    allGoal.append (objectFileForChecking)
 #--- Compile source
    rule = makefile.Rule ([objectFile], "Compiling " + source)
    rule.mOpenSourceOnError = False
    rule.mCommand += COMPILER_TOOL_WITH_OPTIONS
    rule.mCommand += common_definitions.C_Cpp_optimizationOptions ()
    rule.mCommand += common_definitions.Cpp_actualOptions (usesLTO)
    rule.mCommand += ["-g"]
    rule.mCommand += ["-c", sourcePath]
    rule.mCommand += ["-o", objectFile]
    rule.mCommand += ["-DSTATIC=static __attribute__((unused))"] if GROUP_SOURCES else ["-DSTATIC="]
    rule.mCommand += includeDirsInCompilerCommand
    rule.mCommand += ["-MD", "-MP", "-MF", objectFile + ".dep"]
    rule.mDependences.append (allHeaders_file)
    rule.mDependences.append (sourcePath)
    rule.mDependences.append ("makefile.json")
    rule.enterSecondaryDependanceFile (objectFile + ".dep", make)
    make.addRule (rule)
    objectFileList.append (objectFile)
  #--- objdump python source
    objdumpPythonFile = BUILD_DIR + "/" + source + ".objdump.py"
    rule = makefile.Rule ([objdumpPythonFile], "Building " + source + ".objdump.py")
    rule.mDependences.append (objectFile)
    rule.mDependences.append ("makefile.json")
    rule.mCommand += ["../../dev-files/build_objdump.py", OBJDUMP_TOOL, source, objdumpPythonFile]
    rule.mPriority = -1
    make.addRule (rule)
    allGoal.append (objdumpPythonFile)
  #--- AS rule
    rule = makefile.Rule ([asObjectFile], "Compiling -> s " + source)
    rule.mOpenSourceOnError = False
    rule.mCommand += COMPILER_TOOL_WITH_OPTIONS
    rule.mCommand += common_definitions.C_Cpp_optimizationOptions ()
    rule.mCommand += common_definitions.Cpp_actualOptions (usesLTO)
    rule.mCommand += ["-S", sourcePath]
    rule.mCommand += ["-o", asObjectFile]
    rule.mCommand += ["-DSTATIC="]
    rule.mCommand += includeDirsInCompilerCommand
    rule.mCommand += ["-MD", "-MP", "-MF", asObjectFile + ".dep"]
    rule.mDependences.append (sourcePath)
    rule.mDependences.append (allHeaders_file)
    rule.mDependences.append ("makefile.json")
    rule.enterSecondaryDependanceFile (asObjectFile + ".dep", make)
    make.addRule (rule)
  #--- AS rule, getting output assembler file
    listingFile = ASBUILD_DIR + "/" + source + ".s.list"
    rule = makefile.Rule ([listingFile], "Assembling -> listing " + source)
    rule.mOpenSourceOnError = False
    rule.mCommand += AS_TOOL_WITH_OPTIONS
    rule.mCommand += [asObjectFile]
    rule.mCommand += ["-o", "/dev/null"]
    rule.mCommand += ["-aln=" + listingFile]
    rule.mDependences.append (asObjectFile)
    rule.mDependences.append (allHeaders_file)
    rule.mDependences.append ("makefile.json")
    make.addRule (rule)
    asObjectFileList.append (listingFile)
#-- Add ARM S files
  for sourcePath in S_SOURCE_LIST :
    source = os.path.basename (sourcePath)
    objectFile = BUILD_DIR + "/" + source + ".o"
    objectFileForChecking = BUILD_DIR + "/" + source + ".check.o"
    asObjectFile = ASBUILD_DIR + "/" + source + ".s"
    if sourcePath != "" :
      rule = makefile.Rule ([objectFile], "Assembling " + source)
      rule.mOpenSourceOnError = False
      rule.mCommand += AS_TOOL_WITH_OPTIONS
      rule.mCommand += [sourcePath]
      rule.mCommand += ["-o", objectFile]
      rule.mCommand += includeDirsInCompilerCommand
      rule.mCommand += ["--MD", objectFile + ".dep"]
      rule.mDependences.append (sourcePath)
      rule.mDependences.append ("makefile.json")
      rule.enterSecondaryDependanceFile (objectFile + ".dep", make)
      make.addRule (rule)
      objectFileList.append (objectFile)
    #--- Add listing file
      listingFile = ASBUILD_DIR + "/" + source + ".list"
      rule = makefile.Rule ([listingFile], "Assembling -> listing " + source)
      rule.mOpenSourceOnError = False
      rule.mCommand += AS_TOOL_WITH_OPTIONS
      rule.mCommand += [sourcePath]
      rule.mCommand += ["-o", "/dev/null"]
      rule.mCommand += ["-aln=" + listingFile]
      rule.mDependences.append (sourcePath)
      rule.mDependences.append ("makefile.json")
      make.addRule (rule)
      asObjectFileList.append (listingFile)
#--------------------------------------------------------------------------- Link for internal flash
  PRODUCT_FLASH = PRODUCT_DIR + "/product"
  LINKER_SCRIPT_INTERNAL_FLASH = "../../dev-files/" + linkerScript
  allGoal.append (PRODUCT_FLASH + ".elf")
#--- Add link rule
  rule = makefile.Rule ([PRODUCT_FLASH + ".elf"], "Linking " + PRODUCT_FLASH + ".elf")
  rule.mDependences += objectFileList
  rule.mDependences.append (LINKER_SCRIPT_INTERNAL_FLASH)
  rule.mDependences.append ("makefile.json")
  rule.mCommand += LD_TOOL_WITH_OPTIONS
  rule.mCommand += objectFileList
  rule.mCommand += ["-T" + LINKER_SCRIPT_INTERNAL_FLASH]
  rule.mCommand.append ("-Wl,-Map=" + PRODUCT_FLASH + ".map")
  rule.mCommand += common_definitions.commonLinkerFlags (usesLTO)
  rule.mCommand += ["-o", PRODUCT_FLASH + ".elf"]
  make.addRule (rule)
#--- Add hex rule
#   allGoal.append (PRODUCT_FLASH + ".hex")
#   rule = makefile.Rule ([PRODUCT_FLASH + ".hex"], "Hexing " + PRODUCT_FLASH + ".hex")
#   rule.mDependences.append (PRODUCT_FLASH + ".elf")
#   rule.mDependences.append ("makefile.json")
#   rule.mCommand += OBJCOPY_TOOL_WITH_OPTIONS
#   rule.mCommand.append ("-O")
#   rule.mCommand.append ("ihex")
#   rule.mCommand.append (PRODUCT_FLASH + ".elf")
#   rule.mCommand.append (PRODUCT_FLASH + ".hex")
#   make.addRule (rule)
#--- Add bin rule
#   allGoal.append (PRODUCT_FLASH + ".bin")
#   rule = makefile.Rule ([PRODUCT_FLASH + ".bin"], "Converting hex to bin " + PRODUCT_FLASH + ".hex")
#   rule.mDependences.append (PRODUCT_FLASH + ".hex")
#   rule.mDependences.append ("makefile.json")
#   rule.mCommand += OBJCOPY_TOOL_WITH_OPTIONS
#   rule.mCommand.append ("-O")
#   rule.mCommand.append ("binary")
#   rule.mCommand.append (PRODUCT_FLASH + ".elf")
#   rule.mCommand.append (PRODUCT_FLASH + ".bin")
#   make.addRule (rule)
#--- Add uf2 rule
  allGoal.append (PRODUCT_FLASH + ".uf2")
  rule = makefile.Rule ([PRODUCT_FLASH + ".uf2"], "Converting elf to UF2 " + PRODUCT_FLASH + ".elf")
  rule.mDependences.append (PRODUCT_FLASH + ".elf")
  rule.mDependences.append ("makefile.json")
  rule.mCommand.append (ELF2UF2_TOOL_PATH)
  rule.mCommand.append (PRODUCT_FLASH + ".elf")
  rule.mCommand.append (PRODUCT_FLASH + ".uf2")
#   rule.mCommand.append ("../../dev-files/uf2conv.py")
#   rule.mCommand.append ("--convert")
#   rule.mCommand.append ("--family")
#   rule.mCommand.append ("RP2040")
#   rule.mCommand.append ("--base")
#   rule.mCommand.append ("0x2000")
#   rule.mCommand.append ("-o")
#   rule.mCommand.append (PRODUCT_FLASH + ".uf2")
#   rule.mCommand.append (PRODUCT_FLASH + ".bin")
  make.addRule (rule)
#--------------------------------------------------------------------------- Goals
  make.addGoal ("all", allGoal, "Build all")
  make.addGoal ("run", allGoal, "Building all and run")
  make.addGoal ("view-hex", allGoal, "Building all and show hex")
  make.addGoal ("display-obj-size", allGoal, "Build binaries and display object sizes")
  make.addGoal ("as", asObjectFileList, "Compile C and C++ to assembly")
#--------------------------------------------------------------------------- Run jobs
  #make.printRules ()
  #make.checkRules ()
#   make.writeRuleDependancesInDotFile ("dependances.dot")
  make.runGoal (maxConcurrentJobs, showCommand)
#--------------------------------------------------------------------------- Ok ?
  make.printErrorCountAndExitOnError ()
#---------------------------------------------------------------------------- "display-obj-size"
  if GOAL == "display-obj-size" :
    makefile.runCommand (DISPLAY_OBJ_SIZE_TOOL + objectFileList + ["-t"], "Display Object Size", False, showCommand)
#---------------------------------------------------------------------------- "All" or "run"
  if (GOAL == "all") or (GOAL == "run") or (GOAL == "view-hex") :
    s = runProcessAndGetOutput (DISPLAY_OBJ_SIZE_TOOL + ["-t"] + [PRODUCT_FLASH + ".elf"])
    secondLine = s.split('\n')[1]
    numbers = [int(s) for s in secondLine.split() if s.isdigit()]
    print ("  ROM code:    " + str (numbers [0]) + " bytes")
    print ("  ROM data:    " + str (numbers [1]) + " bytes")
    print ("  RAM + STACK: " + str (numbers [2]) + " bytes")
#----------------------------------------------- Run ?
  if GOAL == "run":
    FLASH_PI_PICO = ["../../dev-files/uf2conv.py", "-d", "/dev/cu.usbmodem14301", "--deploy", PRODUCT_FLASH + ".uf2"]
    print (makefile.BOLD_BLUE () + "Flashing Raspberry Pi Pico..." + makefile.ENDC ())
    runProcess (FLASH_PI_PICO)
    print (makefile.BOLD_GREEN () + "Success" + makefile.ENDC ())
  elif GOAL == "view-hex":
    print (makefile.BOLD_GREEN () + "View hex..." + makefile.ENDC ())
    scriptDir = os.path.dirname (os.path.abspath (__file__))
    runProcess (["python", scriptDir+ "/view-hex.py", PRODUCT_FLASH + ".hex"])

#---------------------------------------------------------------------------------------------------
