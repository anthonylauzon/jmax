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

all: $(LIBDIR)/libUtils.a

$(LIBDIR)/libUtils.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libUtils.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libUtils.a

TOCLEAN = $(LIBDIR)/libUtils.a

include $(ROOTDIR)/Makefiles/Makefile.misc
