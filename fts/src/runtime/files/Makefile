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
#

ROOTDIR = ../../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

all: $(LIBDIR)/libFiles.a

$(LIBDIR)/libFiles.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libFiles.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libFiles.a

TOCLEAN = $(LIBDIR)/libFiles.a

include $(ROOTDIR)/Makefiles/Makefile.misc
