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

all: $(LIBDIR)/libsources.a $(LIBDIR)/libsources.so

$(LIBDIR)/libsources.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libsources.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libsources.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

