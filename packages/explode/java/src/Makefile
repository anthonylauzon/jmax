ROOTDIR = ../../../..
PACKAGE_DIR =  


include $(ROOTDIR)/packages/explode/java/Makefiles/Makefile.common

ALL_PACKAGES =  ircam.jmax.editors.explode

all: 
	(cd ircam ; $(MAKE) all)
.PHONY: all

doc:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javadoc -d $(PACKAGE_DIR)/java/doc $(ALL_PACKAGES))
.PHONY: doc

doc_private:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javadoc -private -d $(ROOTDIR)/java/doc_private $(ALL_PACKAGES))
.PHONY: doc_private

jar:  
	(cd $(ROOTDIR)/packages/explode/java/classes ; /bin/rm -f explode.jar ; jar -cvf explode.jar ircam	)
.PHONY: jar


include $(ROOTDIR)/packages/explode/java/Makefiles/Makefile.misc
