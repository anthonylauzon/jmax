#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
#


ROOTDIR = ../../../../../..
PACKAGE_DIR = ircam/jmax/editors/console

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

ConsoleModule:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConsoleModule.java)
.PHONY: ConsoleModule.java


Console:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Console.java)
.PHONY: Console

ConsoleKeyListener:
	  (CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; ConsoleKeyListener.java)
.PHONY: ConsoleKeyListener

ConsoleWriter:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConsoleWriter.java)
.PHONY: ConsoleWriter

ConsoleWindow:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConsoleWindow.java)
.PHONY: ConsoleWindow

ConsoleThread:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConsoleThread.java)
.PHONY: ConsoleThread

ConsoleTextArea:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConsoleTextArea.java)
.PHONY: ConsoleTextArea

include $(ROOTDIR)/java/Makefiles/Makefile.misc
