ROOTDIR = ../..
PACKAGE_DIR = 

include $(ROOTDIR)/java/Makefiles/Makefile.common

BASIC_PACKAGES =  ircam.jmax ircam.jmax.tcl ircam.jmax.utils ircam.jmax.dialogs

FTS_PACKAGES = ircam.jmax.fts ircam.jmax.fts.tcl
EDITORS_PACKAGES = ircam.jmax.editors.console ircam.jmax.editors.console.tcl ircam.jmax.editors.project 
ERMES_PACKAGES = ircam.jmax.editors.ermes ircam.jmax.editors.ermes.tcl
ALL_PACKAGES = $(BASIC_PACKAGES) $(FTS_PACKAGES) $(EDITORS_PACKAGES) $(ERMES_PACKAGES)

all: 
	(cd ircam ; $(MAKE) all)
.PHONY: all

doc:  
	(CLASSPATH=$(JMAX_CLASSPATH); export CLASSPATH ; javadoc -d $(ROOTDIR)/java/doc $(ALL_PACKAGES))
.PHONY: doc

doc_private:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javadoc -private -d $(ROOTDIR)/java/doc_private $(ALL_PACKAGES))
.PHONY: doc_private

jar:  
	(cd $(ROOTDIR)/java/classes ; /bin/rm -f jmax.jar ; jar -cvf ../classes/jmax.jar ircam )
.PHONY: jar

include $(ROOTDIR)/java/Makefiles/Makefile.misc



