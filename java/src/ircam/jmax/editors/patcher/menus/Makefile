# 
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
# 


ROOTDIR = ../../../../../../..
PACKAGE_DIR = ircam/jmax/editors/patcher/menus

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

PatcherMenu:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) PatcherMenu.java)
.PHONY: PatcherMenu

AlignMenu:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) AlignMenu.java)
.PHONY: AlignMenu

EditMenu:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) EditMenu.java)
.PHONY: EditMenu

FileMenu:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) FileMenu.java)
.PHONY: FileMenu

HelpMenu:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) HelpMenu.java)
.PHONY: HelpMenu

TextMenu:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TextMenu.java)
.PHONY: TextMenu


include $(ROOTDIR)/java/Makefiles/Makefile.misc



