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
	javac $(JFLAGS) $(SOURCES)
.PHONY: all

ConnectionDialog:
	javac $(JFLAGS) ConnectionDialog.java
.PHONY: ConnectionDialog.java

ErrorDialog:
	javac $(JFLAGS) ErrorDialog.java
.PHONY: ErrorDialog.java

FileNotSavedDialog:
	javac $(JFLAGS) FileNotSavedDialog.java
.PHONY: FileNotSavedDialog.java

SplashDialog:
	javac $(JFLAGS) SplashDialog.java
.PHONY: SplashDialog.java

SplashDialogThread:
	javac $(JFLAGS) SplashDialogThread.java
.PHONY: SplashDialogThread.java

StatisticsDialog:
	javac $(JFLAGS) StatisticsDialog.java
.PHONY: StatisticsDialog.java

include $(ROOTDIR)/java/Makefiles/Makefile.misc



