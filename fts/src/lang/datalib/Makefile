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

all: $(LIBDIR)/libDatalib.a

$(LIBDIR)/libDatalib.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libDatalib.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libDatalib.a

TOCLEAN = $(LIBDIR)/libDatalib.a

include $(ROOTDIR)/Makefiles/Makefile.misc
