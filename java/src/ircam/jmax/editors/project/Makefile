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
PACKAGE_DIR = ircam/jmax/editors/project

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

all: 
	javac $(JFLAGS) $(SOURCES)
.PHONY: all

Project:
	javac $(JFLAGS) Project.java
.PHONY: Project

ProjectBar:
	javac $(JFLAGS) ProjectBar.java
.PHONY: ProjectBar

ProjectEntry:
	javac $(JFLAGS) ProjectEntry.java
.PHONY: ProjectEntry

ProjectNewDialog:
	javac $(JFLAGS) ProjectNewDialog.java
.PHONY: ProjectNewDialog

ProjectWindow:
	javac $(JFLAGS) ProjectWindow.java
.PHONY: ProjectWindow

include $(ROOTDIR)/java/Makefiles/Makefile.misc
