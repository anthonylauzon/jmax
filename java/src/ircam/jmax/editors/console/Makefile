#
#                      FTS 2.x makefile for GNU Make
#
#			Copyright (c) 1997 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
#


ROOTDIR = ../../../../../..
PACKAGE_DIR = ircam/jmax/editors/console

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

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

ConsoleDocument:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConsoleDocument.java)
.PHONY: ConsoleDocument

include $(ROOTDIR)/java/Makefiles/Makefile.misc
