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
include Sources

all: 
	javac $(JFLAGS) $(SOURCES)
.PHONY: all



ExtensionFilter:
	javac $(JFLAGS) ExtensionFilter.java
.PHONY: ExtensionFilter

TWOutputStream:
	javac $(JFLAGS) TWOutputStream.java
.PHONY: TWOutputStream

TWPrintStream:
	javac $(JFLAGS) TWPrintStream.java
.PHONY: TWPrintStream

TextWindow:
	javac $(JFLAGS) TextWindow.java
.PHONY: TextWindow

Platform:
	javac $(JFLAGS) Platform.java
.PHONY: Platform

IndentedPrintWriter:
	javac $(JFLAGS) IndentedPrintWriter.java
.PHONY: IndentedPrintWriter

include $(ROOTDIR)/java/Makefiles/Makefile.misc
