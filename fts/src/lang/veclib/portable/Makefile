#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
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
