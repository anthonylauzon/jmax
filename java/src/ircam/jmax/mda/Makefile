#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
#


ROOTDIR = ../../../../..
PACKAGE_DIR = ircam/jmax/mda

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

MaxData:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxData.java)
.PHONY: MaxData

MaxDataEditor:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataEditor.java)
.PHONY: MaxDataEditor

MaxDataEditorFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataEditorFactory.java)
.PHONY: MaxDataEditorFactory

MaxDocument:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDocument.java)
.PHONY: MaxDocument

MaxDocumentException:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDocumentException.java)
.PHONY: MaxDocumentException

MaxDocumentHandler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDocumentHandler.java)
.PHONY: MaxDocumentHandler

MaxDocumentType:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDocumentType.java)
.PHONY: MaxDocumentType

MaxTclDocument:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclDocument.java)
.PHONY: MaxTclDocument

MaxTclFileDocumentHandler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclFileDocumentHandler.java)
.PHONY: MaxTclFileDocumentHandler

MaxTclInterpreter:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclInterpreter.java)
.PHONY: MaxTclInterpreter

Mda:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Mda.java)
.PHONY: Mda

MdaModule:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MdaModule.ja)
.PHONY: MdaModule

MaxEnv:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxEnv.java)
.PHONY: MaxEnv

include $(ROOTDIR)/java/Makefiles/Makefile.misc




