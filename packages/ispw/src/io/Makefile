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

ifeq ($(ARCH), solaris2)
MORESOLIBS=/opt/LOCAL/sol/lib/libST.so /usr/demo/SOUND/lib/libaudio.a
endif

all: $(LIBDIR)/libio.a $(LIBDIR)/libio.so

$(LIBDIR)/libio.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libio.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libio.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

