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
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libsignal.a $(LIBDIR)/libsignal.so

$(LIBDIR)/libsignal.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libsignal.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libsignal.a $(LIBDIR)/libsignal.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

