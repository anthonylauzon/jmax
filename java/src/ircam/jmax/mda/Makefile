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
PACKAGE_DIR = ircam/jmax/mda

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

MaxData:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxData.java)
.PHONY: MaxData

MaxDataException:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataException.java)
.PHONY: MaxDataException

MaxDataHandler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataHandler.java)
.PHONY: MaxDataHandler

MaxDataSource:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataSource.java)
.PHONY: MaxDataSource

MaxDataType:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataType.java)
.PHONY: MaxDataType

MaxFileDataHandler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxFileDataHandler.java)
.PHONY: MaxFileDataHandler

MaxFileDataSource:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxFileDataSource.java)
.PHONY: MaxFileDataSource

MaxFtsDataSource:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxFtsDataSource.java)
.PHONY: MaxFtsDataSource

MaxURLDataHandler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxURLDataHandler.java)
.PHONY: MaxURLDataHandler

MaxURLDataSource:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxURLDataSource.java)
.PHONY: MaxURLDataSource

MaxDataEditor:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataEditor.java)
.PHONY: MaxDataEditor

MaxEditorFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxEditorFactory.java)
.PHONY: MaxEditorFactory

MaxTclEditorFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclEditorFactory.java)
.PHONY:  MaxTclEditorFactory


MaxTclData:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclData.java)
.PHONY:  MaxTclData

MaxTclInterpreter:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclInterpreter.java)
.PHONY:  MaxTclInterpreter

include $(ROOTDIR)/java/Makefiles/Makefile.misc




