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

all: $(LIBDIR)/libfilters.a $(LIBDIR)/libfilters.so

$(LIBDIR)/libfilters.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libfilters.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libfilters.a $(LIBDIR)/libfilters.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

