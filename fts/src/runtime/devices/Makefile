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

all: $(LIBDIR)/libDevices.a

$(LIBDIR)/libDevices.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libDevices.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libDevices.a

include $(ROOTDIR)/Makefiles/Makefile.misc
