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

TableExtension:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) TableExtension.java )
.PHONY: TableExtension

include $(ROOTDIR)/packages/table/java/Makefiles/Makefile.misc


