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
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all



MaxFtsConnectCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxFtsConnectCmd.java)
.PHONY: MaxFtsConnectCmd

MaxOpenCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxOpenCmd.java)
.PHONY: MaxOpenCmd

MaxQuitCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxQuitCmd.java)
.PHONY: MaxQuitCmd

MaxRunHooksCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxRunHooksCmd.java)
.PHONY: MaxRunHooksCmd

MaxSystemPropertyCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxSystemPropertyCmd.java)
.PHONY: MaxSystemPropertyCmd

MaxWhenCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxWhenCmd.java)
.PHONY: MaxWhenCmd

MaxOpenConsoleCmd:	
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxOpenConsoleCmd.java)
.PHONY: MaxOpenConsoleCmd

MaxSourceCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxSourceCmd.java)
.PHONY: MaxSourceCmd

MaxSysSourceCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxSysSourceCmd.java)
.PHONY: MaxSysSourceCmd

MaxSplashCmd:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxSplashCmd.java)
.PHONY: MaxSplashCmd

TclMaxPackage:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TclMaxPackage.java)
.PHONY: TclMaxPackage


include $(ROOTDIR)/java/Makefiles/Makefile.misc




