#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
ROOTDIR = ../../..
PACKAGE_DIR = ircam

include $(ROOTDIR)/java/Makefiles/Makefile.common
INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)/jmax

include $(INCLUDE_PATH)/Sources2

all: 
	( cd jmax ; $(MAKE) all )
.PHONY: all

include $(ROOTDIR)/java/Makefiles/Makefile.misc
