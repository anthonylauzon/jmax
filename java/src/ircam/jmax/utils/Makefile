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
PACKAGE_DIR = ircam/jmax/utils

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all



ExtensionFilter:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ExtensionFilter.java)
.PHONY: ExtensionFilter

TextWindow:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TextWindow.java)
.PHONY: TextWindow

Platform:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Platform.java)
.PHONY: Platform

IndentedPrintWriter:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) IndentedPrintWriter.java)
.PHONY: IndentedPrintWriter

Probe:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Probe.java)
.PHONY: Probe

GlobalProbe:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) GlobalProbe.java)
.PHONY: GlobalProbe

ListObject:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ListObject.java)
.PHONY: ListObject

List:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) List.java)
.PHONY: List

ListIterator:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ListIterator.java)
.PHONY: ListIterator

MetaList:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MetaList.java)
.PHONY: MetaList

MetaListIterator:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MetaListIterator.java)
.PHONY: MetaListIterator

Enumeration2:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Enumeration2.java)
.PHONY: Enumeration2

Oscilloscope:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Oscilloscope.java)
.PHONY: Oscilloscope

OscillSource:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) OscillSource.java)
.PHONY: OscillSource

MaxVector:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxVector.java)
.PHONY: MaxVector

include $(ROOTDIR)/java/Makefiles/Makefile.misc




