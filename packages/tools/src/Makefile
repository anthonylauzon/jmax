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
MODULE_CFLAGS= 

DISTDIR = ../../../fts
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libtools.a $(LIBDIR)/libtools.so

$(LIBDIR)/libtools.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libtools.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libtools.a $(LIBDIR)/libtools.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

