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

ROOTDIR= ..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

LIB=$(LIBDIR)/libProtocol.a

all: $(LIB)

$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)

TOCLEAN= $(LIB)

include $(ROOTDIR)/Makefiles/Makefile.misc
