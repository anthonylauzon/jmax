ROOTDIR = ../../..
PACKAGE_DIR = ircam

include $(ROOTDIR)/java/Makefiles/Makefile.common
INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)/jmax

include $(INCLUDE_PATH)/Sources2
all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

include $(ROOTDIR)/java/Makefiles/Makefile.misc
