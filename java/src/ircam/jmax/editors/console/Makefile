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
	javac $(JFLAGS) $(SOURCES)
.PHONY: all

ConsShell:
	javac $(JFLAGS) ConsShell.java
.PHONY: ConsShell

Console:
	javac $(JFLAGS) Console.java
.PHONY: Console


include $(ROOTDIR)/java/Makefiles/Makefile.misc
