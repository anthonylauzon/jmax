#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
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
