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


ROOTDIR = ../../../../../../..
PACKAGE_DIR = ircam/jmax/editors/console/tcl

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

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

