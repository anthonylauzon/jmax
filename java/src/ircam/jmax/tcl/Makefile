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
PACKAGE_DIR = ircam/jmax/tcl

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

all: 
	javac $(JFLAGS) $(SOURCES)
.PHONY: all



MaxFtsConnectCmd:
	javac $(JFLAGS) MaxFtsConnectCmd.java
.PHONY: MaxFtsConnectCmd

MaxOpenCmd:
	javac $(JFLAGS) MaxOpenCmd.java
.PHONY: MaxOpenCmd

MaxPostCmd:
	javac $(JFLAGS) MaxPostCmd.java
.PHONY: MaxPostCmd

MaxQuitCmd:
	javac $(JFLAGS) MaxQuitCmd.java
.PHONY: MaxQuitCmd

MaxRunHooksCmd:
	javac $(JFLAGS) MaxRunHooksCmd.java
.PHONY: MaxRunHooksCmd

MaxSystemPropertyCmd:
	javac $(JFLAGS) MaxSystemPropertyCmd.java
.PHONY: MaxSystemPropertyCmd

MaxWhenCmd:
	javac $(JFLAGS) MaxWhenCmd.java
.PHONY: MaxWhenCmd

TclMaxPackage:
	javac $(JFLAGS) TclMaxPackage.java
.PHONY: TclMaxPackage

include $(ROOTDIR)/java/Makefiles/Makefile.misc
