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

all: $(LIBDIR)/libDevices.a

$(LIBDIR)/libDevices.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libDevices.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libDevices.a

TOCLEAN = $(LIBDIR)/libDevices.a

include $(ROOTDIR)/Makefiles/Makefile.misc
