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
include Sources

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

ConnectionDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ConnectionDialog.java)
.PHONY: ConnectionDialog.java

ErrorDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErrorDialog.java)
.PHONY: ErrorDialog.java

FileNotSavedDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) FileNotSavedDialog.java)
.PHONY: FileNotSavedDialog.java

QuitDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) QuitDialog.java)
.PHONY: QuitDialog.java

SplashDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) SplashDialog.java)
.PHONY: SplashDialog.java

SplashDialogThread:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) SplashDialogThread.java)
.PHONY: SplashDialogThread.java

StatisticsDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) StatisticsDialog.java)
.PHONY: StatisticsDialog.java

MaxFileChooser:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxFileChooser.java)
.PHONY: MaxFileChooser.java

include $(ROOTDIR)/java/Makefiles/Makefile.misc



