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

ROOTDIR = ../../../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

all: $(LIBDIR)/libPortableVecLib.a

$(LIBDIR)/libPortableVecLib.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libPortableVecLib.a $(OBJECTS)

TOCLEAN = $(LIBDIR)/libPortableVecLib.a

include $(ROOTDIR)/Makefiles/Makefile.misc
