#! /usr/bin/env python3
# -*- coding: UTF-8 -*-

#---------------------------------------------------------------------------------------------------

import os

#---------------------------------------------------------------------------------------------------

def createAndGetArchiveDirectory () :
  ARCHIVE_DIR = os.path.abspath (os.path.dirname (__file__) + "/../archives")
  if not os.path.exists (ARCHIVE_DIR):
    os.mkdir (ARCHIVE_DIR)
  return ARCHIVE_DIR

#---------------------------------------------------------------------------------------------------

