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
PACKAGE_DIR = ircam/jmax/editors/console/tcl

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all


ConsoleOpenCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConsoleOpenCmd.java)
.PHONY: ConsoleOpenCmd

TclConsolePackage:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TclConsolePackage.java)
.PHONY: TclConsolePackage


include $(ROOTDIR)/java/Makefiles/Makefile.misc

