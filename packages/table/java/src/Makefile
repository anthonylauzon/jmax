ROOTDIR = ../../../..
PACKAGE_DIR =  


include $(ROOTDIR)/packages/table/java/Makefiles/Makefile.common

ALL_PACKAGES =  ircam.jmax.editors.table

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
	(cd $(ROOTDIR)/packages/table/java/classes ; /bin/rm -f table.jar ; jar -cvf table.jar ircam	)
.PHONY: jar


include $(ROOTDIR)/packages/table/java/Makefiles/Makefile.misc
