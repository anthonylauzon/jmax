#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  

JMAXROOTDIR=../../../..
DISTDIR=$(JMAXROOTDIR)/fts

PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

MODULE_CFLAGS= -I$(JMAXROOTDIR)/lib/protocol/c/src
MORESOLIBS=$(JMAXROOTDIR)/lib/protocol/c/lib/$(ARCH)/libProtocol.a

all: $(LIBDIR)/libio.a $(LIBDIR)/libio.so

$(LIBDIR)/libio.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libio.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libio.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

