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

DISTDIR = ../../../fts
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources


all: $(LIBDIR)/libdebug.a $(LIBDIR)/libdebug.so

$(LIBDIR)/libdebug.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libdebug.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libdebug.a

TOCLEAN = $(LIBDIR)/libdebug.a $(LIBDIR)/libdebug.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

