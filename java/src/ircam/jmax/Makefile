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


ROOTDIR = ../../../..
PACKAGE_DIR = ircam/jmax

include $(ROOTDIR)/java/Makefiles/Makefile.common

all:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) -depend MaxApplication.java)
	(cd toolkit ; $(MAKE) needed)
.PHONY: all


TestWindow:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) -deprecation TestWindow.java)
.PHONY: TestWindow

MaxApplication:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxApplication.java)
.PHONY: MaxApplication

MaxEditor:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxEditor.java)
.PHONY: MaxEditor

MaxWhenHookTable:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxWhenHookTable.java)
.PHONY: MaxWhenHookTable

MaxWindowManager:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxWindowManager.java)
.PHONY: MaxWindowManager

MaxVersion:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxVersion.java)
.PHONY: MaxVersion

include $(ROOTDIR)/java/Makefiles/Makefile.misc


