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

ROOTDIR = ../../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources


all: portable $(LIBDIR)/libVecLib.a

clean : portable_clean

portable:
	(cd portable; $(MAKE) all)	
.PHONY: portable

portable_clean:
	(cd portable; $(MAKE) clean)	
.PHONY: portable_clean

$(LIBDIR)/libVecLib.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libVecLib.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libVecLib.a

TOCLEAN = $(LIBDIR)/libVecLib.a

include $(ROOTDIR)/Makefiles/Makefile.misc
