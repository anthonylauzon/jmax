ROOTDIR = ../../../../../../../..
PACKAGE_DIR = ircam/jmax/editors/explode

include $(ROOTDIR)/packages/explode/java/Makefiles/Makefile.common

include Sources

all:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

Explode:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) -depend Explode.java )
.PHONY: Explode

ExplodeFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ExplodeFactory.java )
.PHONY: ExplodeFactory

ExplodeDataEditor:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ExplodeDataEditor.java )
.PHONY: ExplodeDataEditor

ExplodeExtension:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ExplodeExtension.java )
.PHONY: ExplodeExtension

ScrPanel:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ScrPanel.java )
.PHONY: ScrPanel

include $(ROOTDIR)/packages/explode/java/Makefiles/Makefile.misc


