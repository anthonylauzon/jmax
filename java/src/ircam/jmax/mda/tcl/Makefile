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
PACKAGE_DIR = ircam/jmax/mda/tcl

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

all: 
	javac $(JFLAGS) $(SOURCES)
.PHONY: all

MaxData:
	javac $(JFLAGS) MaxData.java
.PHONY: MaxData


InstallEditorForCmd:
	javac $(JFLAGS) InstallEditorForCmd.java
.PHONY: InstallEditorForCmd

TclMdaPackage:
	javac $(JFLAGS) TclMdaPackage.java
.PHONY: TclMdaPackage

JMaxCmd:
	javac $(JFLAGS) JMaxCmd.java
.PHONY: JMaxCmd


include $(ROOTDIR)/java/Makefiles/Makefile.misc




