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

all: $(LIBDIR)/libSTtools.a

$(LIBDIR)/libSTtools.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libSTtools.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libSTtools.a

TOCLEAN = $(LIBDIR)/libSTtools.a

include $(ROOTDIR)/Makefiles/Makefile.misc





