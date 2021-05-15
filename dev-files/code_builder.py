#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
#---------------------------------------------------------------------------------------------------

import sys, os, subprocess, shutil, json

#---------------------------------------------------------------------------------------------------

import makefile
import common_definitions
import download_and_install_gccarm

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
#   dictionaryFromJsonFile
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

def buildCode (GOAL, projectDir, maxConcurrentJobs, verbose):
  DEV_FILES_DIR = os.path.dirname (os.path.realpath (__file__))
#   print ("DEV_FILES_DIR: " + DEV_FILES_DIR)
#--------------------------------------------------------------------------- Prepare
  os.chdir (projectDir)
  make = makefile.Make (GOAL)
#   make.mMacTextEditor = "BBEdit" # "Atom"
  allGoal = []
#--------------------------------------------------------------------------- Analyze JSON file
  print (makefile.BOLD_GREEN () + "--- Making " + projectDir + makefile.ENDC ())
  dictionaire = dictionaryFromJsonFile (projectDir + "/makefile.json")
#   print ("JSON DICTIONARY: ")
#   print (dictionaire)
#--------------------------------------------------------------------------- Find target
  targetNameSet = set ()
  foundIRQSectionScheme = False
  for name in os.listdir (DEV_FILES_DIR + "/targets") :
    if not name.startswith ('.') :
      targetNameSet.add (name)
  if not "TARGET" in dictionaire:
    s = "\"TARGET\" is not defined in the makefile.json file; possible values:\n"
    for target in targetNameSet :
      s += "  -  \"" + target + "\"\n"
    print (makefile.BOLD_RED () + s + makefile.ENDC ())
    sys.exit (1)
  targetName = dictionaire ["TARGET"]
  if not targetName in targetNameSet :
    s = "In the makefile.json file, \"TARGET\" value \"" + targetName + "\" is invalid; "
    s += "possible values:\n"
    for target in targetNameSet :
      s += "  -  \"" + target + "\"\n"
    print (makefile.BOLD_RED () + s + makefile.ENDC ())
    sys.exit (1)
  TARGET_DIR = DEV_FILES_DIR + "/targets/" + targetName
  sys.path.append (TARGET_DIR + "/deployment")
  import deployment
#--------------------------------------------------------------------------- Install compiler ?
  BASE_NAME = "arm-none-eabi"
  TOOL_DIR = download_and_install_gccarm.installGCCARMandGetToolDirectory ()
#--------------------------------------------------------------------------- Target Dictionary
  targetDictionary = dictionaryFromJsonFile (TARGET_DIR + "/helpers/target-parameters.json")
#--------------------------------------------------------------------------- Configure compiler
  gccOptions = targetDictionary ["GCC-OPTIONS"]
  AS_TOOL_WITH_OPTIONS = [TOOL_DIR + "/bin/" + BASE_NAME + "-as"]
  AS_TOOL_WITH_OPTIONS += gccOptions
  COMPILER_TOOL_WITH_OPTIONS = [TOOL_DIR + "/bin/" + BASE_NAME + "-gcc"]
  COMPILER_TOOL_WITH_OPTIONS += gccOptions
#   LD_TOOL_WITH_OPTIONS = [TOOL_DIR + "/bin/" + BASE_NAME + "-ld"]
  LD_TOOL_WITH_OPTIONS = COMPILER_TOOL_WITH_OPTIONS
  # OBJCOPY_TOOL_WITH_OPTIONS = [TOOL_DIR + "/bin/" + BASE_NAME + "-objcopy"]
  DISPLAY_OBJ_SIZE_TOOL = [TOOL_DIR + "/bin/" + BASE_NAME + "-size"]
#   OBJDUMP_TOOL = TOOL_DIR + "/bin/" + BASE_NAME + "-objdump"
#--------------------------------------------------------------------------- Parse JSON dictinary
#--- CPU_MHZ
  CPU_MHZ = 0
  if "CPU-MHZ" in dictionaire:
    CPU_MHZ = dictionaire ["CPU-MHZ"]
#--- ASSERTION_GENERATION
  ASSERTION_GENERATION = False
  if "ASSERTION-GENERATION" in dictionaire :
    ASSERTION_GENERATION = dictionaire ["ASSERTION-GENERATION"]
#--- USER SOURCE_FILE_DIRECTORIES
  SOURCE_FILE_DIRECTORIES = []
  if "SOURCE-DIR" in dictionaire :
    SOURCE_FILE_DIRECTORIES = dictionaire ["SOURCE-DIR"]
#--- SYSTEM SOURCE_FILE_DIRECTORIES
  if "SOURCES-IN-DEV-DIR" in dictionaire :
    sourcesInDevDir = dictionaire ["SOURCES-IN-DEV-DIR"]
    for d in sourcesInDevDir :
      SOURCE_FILE_DIRECTORIES.append (TARGET_DIR + "/" + d)
#--- GROUP_SOURCES
  GROUP_SOURCES = False
  if "GROUP-SOURCES" in dictionaire:
    GROUP_SOURCES = dictionaire ["GROUP-SOURCES"]
#--- TASK_COUNT
  TASK_COUNT = "0" # Means TASK_COUNT is not defined by JSON file
  if "TASK-COUNT" in dictionaire :
    TASK_COUNT = str (dictionaire ["TASK-COUNT"])
#--- LTO
  usesLTO = False
  if "LTO" in dictionaire :
    usesLTO = dictionaire ["LTO"]
#--- SERVICE
  serviceScheme = ""
  if "SERVICE-SCHEME" in dictionaire :
    serviceScheme = dictionaire ["SERVICE-SCHEME"]
#--- SECTION
  sectionScheme = ""
  if "SECTION-SCHEME" in dictionaire :
    sectionScheme = dictionaire ["SECTION-SCHEME"]
#--- IRQ SECTION
  irqSectionScheme = ""
  if "IRQ-SECTION-SCHEME" in dictionaire :
    irqSectionScheme = dictionaire ["IRQ-SECTION-SCHEME"]
#--- UNUSED IRQ
  unusedIRQScheme = ""
  if "UNUSED-IRQ-SCHEME" in dictionaire :
    unusedIRQScheme = dictionaire ["UNUSED-IRQ-SCHEME"]
#--- DEPLOYMENT
  selectedDeployments = []
  if "DEPLOYMENT" in dictionaire :
    selectedDeployments = dictionaire ["DEPLOYMENT"]
#--------------------------------------------------------------------------- Directories
  BUILD_DIR = common_definitions.buildDirectory ()
  GENERATED_SOURCE_DIR = common_definitions.generatedSourceDirectory ()
  PRODUCT_DIR = common_definitions.productDirectory ()
  ASBUILD_DIR = common_definitions.asDirectory ()
#--------------------------------------------------------------------------- Build source lists
  includeDirsInCompilerCommand = ["-I", GENERATED_SOURCE_DIR]
  H_SOURCE_SET = set ()
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
          if sourcePath in H_SOURCE_SET :
            print (makefile.BOLD_RED () + "Duplicated header file \"" + sourcePath + "\"" + makefile.ENDC ())
          H_SOURCE_SET.add (sourcePath)
        elif extension == ".s" :
          S_SOURCE_LIST.append (sourcePath)
        elif extension == ".hs" :
          pass # Ok
        elif extension == ".ld" :
          pass # Ok
        elif extension != "" : # Ceci permet d'ignorer les fichés cachés (dont les noms commencent par un point)
          print (makefile.MAGENTA () + makefile.BOLD () + "Note: unhandled file " + sourcePath + makefile.ENDC ())
#--------------------------------------------------------------------------- Build base header file
  baseHeader_file = GENERATED_SOURCE_DIR + "/base.h"
  H_SOURCE_LIST.insert (0, baseHeader_file)
  rule = makefile.Rule ([baseHeader_file], "Build base header file")
  rule.mOpenSourceOnError = False
  rule.mDependences.append ("makefile.json")
  rule.mCommand += [DEV_FILES_DIR + "/build_base_header_file.py", baseHeader_file, str (CPU_MHZ), TASK_COUNT, targetName, "1" if ASSERTION_GENERATION else "0"]
  rule.mPriority = -1
  make.addRule (rule)
#--------------------------------------------------------------------------- Build all header file
  allHeadersSecondaryDependenceFile = BUILD_DIR + "/all-headers.dep"
  allHeaders_file = GENERATED_SOURCE_DIR + "/all-headers.h"
  rule = makefile.Rule ([allHeaders_file, allHeadersSecondaryDependenceFile], "Build all headers file")
  rule.mOpenSourceOnError = False
  rule.mDependences.append ("makefile.json")
  rule.mDependences += H_SOURCE_LIST
  rule.mCommand += [DEV_FILES_DIR + "/build_all_header_file.py", allHeaders_file, allHeadersSecondaryDependenceFile]
  rule.mCommand += H_SOURCE_LIST
  rule.enterSecondaryDependanceFile (allHeadersSecondaryDependenceFile, make)
  rule.mPriority = -1
  make.addRule (rule)
#--------------------------------------------------------------------------- Build precompiled header file
#   allHeadersSecondaryDependenceFile = GENERATED_SOURCE_DIR + "/all-headers.h"
#   precompiledHeader_file = GENERATED_SOURCE_DIR + "/all-headers.h.gch"
#   rule = makefile.Rule ([precompiledHeader_file], "Build Precompiled header file")
#   rule.mOpenSourceOnError = False
#   rule.mDependences.append ("makefile.json")
#   rule.mDependences.append (allHeadersSecondaryDependenceFile)
#   rule.mCommand += COMPILER_TOOL_WITH_OPTIONS
#   rule.mCommand += ["-DSTATIC=static __attribute__((unused))"] if GROUP_SOURCES else ["-DSTATIC="]
#   rule.mCommand += common_definitions.checkModeOptions ()
#   rule.mCommand += common_definitions.C_Cpp_optimizationOptions ()
#   rule.mCommand += common_definitions.Cpp_actualOptions (False)
#   rule.mCommand += includeDirsInCompilerCommand
#   rule.mCommand += ["-x", "c++-header", allHeadersSecondaryDependenceFile]
#   rule.mCommand += ["-o", precompiledHeader_file]
#   rule.mPriority = -1
#   make.addRule (rule)
#   allGoal.append (precompiledHeader_file)
#--------------------------------------------------------------   --- Build interrupt handler files
  interruptHandlerSFile = GENERATED_SOURCE_DIR + "/interrupt-handlers-assembly.s"
  interruptHandlerCppFile = GENERATED_SOURCE_DIR + "/interrupt-handlers-cpp.cpp"
  S_SOURCE_LIST.append (interruptHandlerSFile)
  CPP_SOURCE_LIST.append (interruptHandlerCppFile)
  rule = makefile.Rule ([interruptHandlerSFile, interruptHandlerCppFile], "Build interrupt files")
  rule.mOpenSourceOnError = False
  rule.mDependences += H_SOURCE_LIST
  rule.mDependences.append ("makefile.json")
  rule.mDependences.append (DEV_FILES_DIR + "/build_interrupt_handlers.py")
  if serviceScheme != "" :
    rule.mDependences.append (TARGET_DIR + "/generators-service/" + serviceScheme + "/service_generator.py")
  if sectionScheme != "" :
    rule.mDependences.append (TARGET_DIR + "/generators-section/" + sectionScheme + "/section_generator.py")
  if irqSectionScheme != "" :
    rule.mDependences.append (TARGET_DIR + "/generators-irq-section/" + irqSectionScheme + "/irq_section_generator.py")
  if unusedIRQScheme != "" :
    rule.mDependences.append (TARGET_DIR + "/generators-unused-irq/" + unusedIRQScheme + "/unused_irq_generator.py")
  rule.mCommand += [DEV_FILES_DIR + "/build_interrupt_handlers.py"]
  rule.mCommand += [TARGET_DIR]
  rule.mCommand += [interruptHandlerCppFile]
  rule.mCommand += [interruptHandlerSFile]
  rule.mCommand += [serviceScheme]
  rule.mCommand += [sectionScheme]
  rule.mCommand += [irqSectionScheme]
  rule.mCommand += [unusedIRQScheme]
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
    rule.mCommand += [DEV_FILES_DIR + "/build_grouped_sources.py", allSourceFile]
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
    rule.mDependences.append (allHeadersSecondaryDependenceFile)
#     rule.mDependences.append (precompiledHeader_file)
    rule.mDependences.append (sourcePath)
    rule.mDependences.append ("makefile.json")
    rule.enterSecondaryDependanceFile (objectFileForChecking + ".dep", make)
    rule.mCommand += COMPILER_TOOL_WITH_OPTIONS
    rule.mCommand += ["-x", "c++"]
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
    rule.mDependences.append (allHeadersSecondaryDependenceFile)
##    rule.mDependences.append (precompiledHeader_file)
    rule.mDependences.append (sourcePath)
    rule.mDependences.append ("makefile.json")
    rule.enterSecondaryDependanceFile (objectFile + ".dep", make)
    make.addRule (rule)
    objectFileList.append (objectFile)
  #--- objdump python3 source
#     objdumpPythonFile = BUILD_DIR + "/" + source + ".objdump.py"
#     rule = makefile.Rule ([objdumpPythonFile], "Building " + source + ".objdump.py")
#     rule.mDependences.append (objectFile)
#     rule.mDependences.append ("makefile.json")
#     rule.mCommand += [DEV_FILES_DIR + "/build_objdump.py", OBJDUMP_TOOL, source, objdumpPythonFile]
#     rule.mPriority = -1
#     make.addRule (rule)
#     allGoal.append (objdumpPythonFile)
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
      rule.mCommand += includeDirsInCompilerCommand
      rule.mCommand += ["-o", "/dev/null"]
      rule.mCommand += ["-aln=" + listingFile]
      rule.mDependences.append (sourcePath)
      rule.mDependences.append ("makefile.json")
      make.addRule (rule)
      asObjectFileList.append (listingFile)
#--------------------------------------------------------------------------- Enumerate deployments
  deploymentDictionary = deployment.deploymentDictionary ()
#--------------------------------------------------------------------- - Check selected deployments
  for selectedDeployment in selectedDeployments :
    if not selectedDeployment in deploymentDictionary.keys () :
      s = "Invalid deployment \"" + selectedDeployment + "\"; possible values:\n"
      for dep in deploymentDictionary.keys () :
        s += "  - \"" + dep + "\"\n"
      print (makefile.BOLD_RED () + s + makefile.ENDC ())
      sys.exit (1)
#--------------------------------------------------------------------------- Build deployment files
  runGoalDictionary = dict ()
  for selectedDeployment in selectedDeployments :
    runGoalDictionary ["run-" + selectedDeployment] = selectedDeployment
    linkerScript = deploymentDictionary [selectedDeployment]
    LINKER_SCRIPT = TARGET_DIR + "/deployment/" + linkerScript
    PRODUCT = PRODUCT_DIR + "/deployment-" + selectedDeployment
    allGoal.append (PRODUCT + ".elf")
  #--- Add link rule
    rule = makefile.Rule ([PRODUCT + ".elf"], "Linking " + PRODUCT + ".elf")
    rule.mDependences += objectFileList
    rule.mDependences.append (LINKER_SCRIPT)
    rule.mDependences.append ("makefile.json")
    rule.mCommand += LD_TOOL_WITH_OPTIONS
    rule.mCommand += objectFileList
    rule.mCommand += ["-T" + LINKER_SCRIPT]
    rule.mCommand.append ("-Wl,-Map=" + PRODUCT + ".map")
    rule.mCommand += common_definitions.commonLinkerFlags (usesLTO)
    rule.mCommand += ["-o", PRODUCT + ".elf"]
    make.addRule (rule)
    make.addGoal ("run-" + selectedDeployment, allGoal, "Building " + selectedDeployment + " deployment and run")
  #--- Add deployment rules
    (goal, rule) = deployment.buildDeployment (PRODUCT, selectedDeployment, verbose)
    allGoal.append (goal)
    make.addRule (rule)
#     sys.path.pop ()
  #--- Write deployment script
    pythonScriptFilePath = projectDir + "/2-run-" + selectedDeployment + "-via-usb.py"
    if not os.path.exists (pythonScriptFilePath) :
      f = open (DEV_FILES_DIR + "/deployment-script.py.txt", "r")
      genericScript = f.read ()
      f.close ()
      pythonScriptContents = genericScript.replace ("DEPLOYMENT", "run-" + selectedDeployment)
      f = open (pythonScriptFilePath, "wt")
      f.write (pythonScriptContents)
      f.close ()
      #---
      mode = os.stat (pythonScriptFilePath).st_mode
      # print (mode)
      mode += 0o0100 # Add execute permission
      # print (mode)
      os.chmod (pythonScriptFilePath, mode)
#--------------------------------------------------------------------------- Goals
  make.addGoal ("all", allGoal, "Build all")
  make.addGoal ("view-hex", allGoal, "Building all and show hex")
  make.addGoal ("display-obj-size", allGoal, "Build binaries and display object sizes")
  make.addGoal ("as", asObjectFileList, "Compile C and C++ to assembly")
#--------------------------------------------------------------------------- Run jobs
  #make.printRules ()
  #make.checkRules ()
#   make.writeRuleDependancesInDotFile ("dependances.dot")
  make.runGoal (maxConcurrentJobs, verbose)
#--------------------------------------------------------------------------- Ok ?
  make.printErrorCountAndExitOnError ()
#---------------------------------------------------------------------------- "display-obj-size"
  if GOAL == "display-obj-size" :
    makefile.runCommand (DISPLAY_OBJ_SIZE_TOOL + objectFileList + ["-t"], "Display Object Size", False, verbose)
#---------------------------------------------------------------------------- "All" or "run"
  if GOAL == "all" :
    for selectedDeployment in selectedDeployments :
      PRODUCT = PRODUCT_DIR + "/deployment-" + selectedDeployment
      s = runProcessAndGetOutput (DISPLAY_OBJ_SIZE_TOOL + ["-t"] + [PRODUCT + ".elf"])
      secondLine = s.split('\n')[1]
      numbers = [int(s) for s in secondLine.split() if s.isdigit()]
      print ("Deployment \"" + selectedDeployment + "\":")
      print ("  Code:             " + str (numbers [0]) + " bytes")
      print ("  Initialized data: " + str (numbers [1]) + " bytes")
      print ("  RAM + STACK:      " + str (numbers [2]) + " bytes")
#---------------------------------------------------------------------------- Run ?
  if GOAL in runGoalDictionary.keys () :
    selectedDeployment = runGoalDictionary [GOAL]
    PRODUCT = PRODUCT_DIR + "/deployment-" + selectedDeployment
    DEPLOYMENT_DIR = TARGET_DIR + "/deployment"
    deployment.performDeployment (DEPLOYMENT_DIR, PRODUCT, selectedDeployment)

#---------------------------------------------------------------------------------------------------
