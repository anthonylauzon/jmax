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

MaxOpenConsoleCmd:	
	javac $(JFLAGS) MaxOpenConsoleCmd.java
.PHONY: MaxOpenConsoleCmd

MaxSourceCmd:
	javac $(JFLAGS) MaxSourceCmd.java
.PHONY: MaxSourceCmd

MaxSysSourceCmd:
	javac $(JFLAGS) MaxSysSourceCmd.java
.PHONY: MaxSysSourceCmd

MaxSplashCmd:
	javac $(JFLAGS) MaxSplashCmd.java
.PHONY: MaxSplashCmd

TclMaxPackage:
	javac $(JFLAGS) TclMaxPackage.java
.PHONY: TclMaxPackage


include $(ROOTDIR)/java/Makefiles/Makefile.misc
