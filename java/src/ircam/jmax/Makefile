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
	javac $(JFLAGS) -depend MaxApplication.java
	(cd editors/ermes ; $(MAKE) dynamics )
.PHONY: all


MaxApplication:
	javac $(JFLAGS) MaxApplication.java
.PHONY: MaxApplication

MaxDocument:
	javac $(JFLAGS) MaxDocument.java
.PHONY: MaxDocument

MaxEditor:
	javac $(JFLAGS) MaxEditor.java
.PHONY: MaxEditor

MaxInterp:
	javac $(JFLAGS) MaxInterp.java
.PHONY: MaxInterp

MaxResourceId:
	javac $(JFLAGS) MaxResourceId.java
.PHONY: MaxResourceId

MaxWindow:
	javac $(JFLAGS) MaxWindow.java
.PHONY: MaxWindow

include $(ROOTDIR)/java/Makefiles/Makefile.misc
