ROOTDIR = ../../../../../../../..
PACKAGE_DIR = ircam/jmax/editors/explode

include $(ROOTDIR)/packages/explode/java/Makefiles/Makefile.common

include Sources

all:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

Explode:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Explode.java )
.PHONY: Explode

ExplodeFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ExplodeFactory.java )
.PHONY: ExplodeFactory

ExplodeExtension:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ExplodeExtension.java )
.PHONY: ExplodeExtension

include $(ROOTDIR)/packages/explode/java/Makefiles/Makefile.misc


