#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
#


ROOTDIR = ../../../../../..
PACKAGE_DIR = ircam/jmax/editors/project

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

Project:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Project.java)
.PHONY: Project

ProjectBar:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ProjectBar.java)
.PHONY: ProjectBar

ProjectEntry:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ProjectEntry.java)
.PHONY: ProjectEntry

ProjectNewDialog:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ProjectNewDialog.java)
.PHONY: ProjectNewDialog

ProjectWindow:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ProjectWindow.java)
.PHONY: ProjectWindow

include $(ROOTDIR)/java/Makefiles/Makefile.misc
