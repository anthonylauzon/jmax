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

ROOTDIR = ../../../..
PACKAGE_DIR = ircam/jmax

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
	(cd editors/patcher ; $(MAKE) all)
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








