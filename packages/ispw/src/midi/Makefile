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


DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libmidi.a $(LIBDIR)/libmidi.so

$(LIBDIR)/libmidi.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libmidi.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libmidi.a $(LIBDIR)/libmidi.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

