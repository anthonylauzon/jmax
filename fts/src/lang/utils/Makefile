#
#                      FTS 2.x makefile for GNU Make
#
#			Copyright (c) 1997 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
#


ROOTDIR = ../../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

all: $(LIBDIR)/libUtils.a

$(LIBDIR)/libUtils.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libUtils.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libUtils.a

TOCLEAN = $(LIBDIR)/libUtils.a

include $(ROOTDIR)/Makefiles/Makefile.misc
