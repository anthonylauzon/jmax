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
PACKAGE_DIR = ircam/jmax/dialogs

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include $(INCLUDE_PATH)/Sources2

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

ErrorDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErrorDialog.java)
.PHONY: ErrorDialog

FileNotSavedDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) FileNotSavedDialog.java)
.PHONY: FileNotSavedDialog

QuitDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) QuitDialog.java)
.PHONY: QuitDialog

SplashDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) SplashDialog.java)
.PHONY: SplashDialog

SplashDialogThread:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) SplashDialogThread.java)
.PHONY: SplashDialogThread

StatisticsDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) StatisticsDialog.java)
.PHONY: StatisticsDialog

MaxFileChooser:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxFileChooser.java)
.PHONY: MaxFileChooser

YesOrNo:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) YesOrNo.java)
.PHONY: YesOrNo

include $(ROOTDIR)/java/Makefiles/Makefile.misc



