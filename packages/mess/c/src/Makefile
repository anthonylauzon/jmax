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

DISTDIR = ../../../../fts
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

MODULE_CFLAGS=-I$(PACKAGEROOT)/../../data/c/src/

## On Linux, the search patch of the linked .so must be given at link time, otherwise
## dynamic link fails.
#MORESOLIBS=$(PACKAGEROOT)/../../data/c/lib/$(ARCH)/$(MODE)/libdata.so
MORESOLIBS=

all: $(LIBDIR) $(OBJDIR) $(LIBDIR)/libmess.a $(LIBDIR)/libmess.so

$(LIBDIR)/libmess.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libmess.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libmess.a $(LIBDIR)/libmess.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

