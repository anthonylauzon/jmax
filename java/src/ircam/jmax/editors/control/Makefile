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
PACKAGE_DIR = ircam/jmax/editors/control

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

ControlModule:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ControlModule.java)
.PHONY: ControlModule.java

DspControl:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) DspControl.java)
.PHONY: DspControl


include $(ROOTDIR)/java/Makefiles/Makefile.misc
