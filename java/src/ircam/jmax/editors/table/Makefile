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
PACKAGE_DIR = ircam/jmax/editors/table

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

#ConsoleModule:
#	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConsoleModule.java)
#.PHONY: ConsoleModule.java


Tabler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Tabler.java)
.PHONY: Tabler

include $(ROOTDIR)/java/Makefiles/Makefile.misc
