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

ROOTDIR = ../../../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

zap:
	echo mav 
	echo libdir $(LIBDIR)
	echo ar $(AR)
	echo arflags $(ARFLAGS)
	echo ranlib $(RANLIB)
	echo ranlib $(ROOTDIR)
.PHONY: zap

all: $(LIBDIR)/libPortableVecLib.a

$(LIBDIR)/libPortableVecLib.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libPortableVecLib.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libPortableVecLib.a

TOCLEAN = $(LIBDIR)/libPortableVecLib.a

include $(ROOTDIR)/Makefiles/Makefile.misc
