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

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

ControlModule:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ControlModule.java)
.PHONY: ControlModule.java

ControlPanelFrame:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ControlPanelFrame.java)
.PHONY: ControlPanelFrame.java

DspControl:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) DspControl.java)
.PHONY: DspControl


include $(ROOTDIR)/java/Makefiles/Makefile.misc
