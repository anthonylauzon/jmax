ROOTDIR = ../..
PACKAGE_DIR = 

include $(ROOTDIR)/java/Makefiles/Makefile.common

BASIC_PACKAGES =  ircam.jmax ircam.jmax.tcl ircam.jmax.utils ircam.jmax.dialogs

FTS_PACKAGES = ircam.jmax.fts ircam.jmax.fts ircam.jmax.exceptions ircam.jmax.fts.comm ircam.jmax.fts.tcl
EDITORS_PACKAGES = ircam.jmax.editors.console ircam.jmax.editors.console.tcl ircam.jmax.editors.project 
ERMES_PACKAGES = ircam.jmax.editors.ermes ircam.jmax.editors.ermes.objects ircam.jmax.editors.ermes.connections ircam.jmax.editors.ermes.tcl
ALL_PACKAGES = $(BASIC_PACKAGES) $(FTS_PACKAGES) $(EDITORS_PACKAGES) $(ERMES_PACKAGES)

all: 
	javac $(JFLAGS) $(ALL_PACKAGES)
.PHONY: all

doc:  
	javadoc 

include $(ROOTDIR)/java/Makefiles/Makefile.misc
