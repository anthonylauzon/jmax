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
DISTDIR = ../../../../fts/
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libguiobj.a $(LIBDIR)/libguiobj.so

$(LIBDIR)/libguiobj.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libguiobj.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libguiobj.a $(LIBDIR)/libguiobj.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

