ROOTDIR = ../../../../../../../..
PACKAGE_DIR = ircam/jmax/editors/table

include $(ROOTDIR)/packages/table/java/Makefiles/Makefile.common

include Sources

all:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

Tabler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Tabler.java )
.PHONY: Tabler

TableFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TableFactory.java )
.PHONY: TableFactory

TableDataEditor:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TableDataEditor.java )
.PHONY: TableDataEditor

TableExtension:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TableExtension.java )
.PHONY: TableExtension

TableRemoteData:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TableRemoteData.java )
.PHONY: TableRemoteData

TablePanel:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TablePanel.java )
.PHONY: TablePanel

LinerTool:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) LinerTool.java )
.PHONY: LinerTool

StripeSelecter:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) StripeSelecter.java )
.PHONY: StripeSelecter

TableCutter:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TableCutter.java )
.PHONY: TableCutter

include $(ROOTDIR)/packages/table/java/Makefiles/Makefile.misc


