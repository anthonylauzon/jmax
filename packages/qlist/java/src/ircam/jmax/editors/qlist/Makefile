ROOTDIR = ../../../../../../../..
PACKAGE_DIR = ircam/jmax/editors/qlist

include $(ROOTDIR)/packages/qlist/java/Makefiles/Makefile.common

include Sources

all:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

QList:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) QList.java )
.PHONY: QList

QListFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) QListFactory.java )
.PHONY: QListFactory

QListExtension:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) QListExtension.java )
.PHONY: QListExtension

QListPanel:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) QListPanel.java )
.PHONY: QListPanel

include $(ROOTDIR)/packages/qlist/java/Makefiles/Makefile.misc


