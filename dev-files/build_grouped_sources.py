#! /usr/bin/env python
# -*- coding: UTF-8 -*-

import sys, os

#---------------------------------------------------------------------------------------------------

separator = "//" + ("-" * 78) + "\n"
#------------------------------ Destination file
destinationFile = sys.argv [1]
# print ("Dest " + destinationFile)
#------------------------------ Header files
s = separator + "\n"
for i in range (2, len (sys.argv)):
  s += "#include \"" + sys.argv [i] + "\"\n"
s += "\n" + separator
#------------------------------ Write destination file
f = open (destinationFile, "wt")
f.write (s)
f.close()

#---------------------------------------------------------------------------------------------------
