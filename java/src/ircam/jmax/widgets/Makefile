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


ROOTDIR = ../../../../..
PACKAGE_DIR = ircam/jmax/widgets

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

ObjectSetViewer:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ObjectSetViewer.java)
.PHONY: ObjectSetViewer

IndicatorWithMemory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) IndicatorWithMemory.java)
.PHONY: IndicatorWithMemory

include $(ROOTDIR)/java/Makefiles/Makefile.misc


