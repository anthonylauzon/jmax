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

all: $(LIBDIR)/libMidi.a

$(LIBDIR)/libMidi.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libMidi.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libMidi.a

include $(ROOTDIR)/Makefiles/Makefile.misc
