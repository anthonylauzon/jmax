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

all: $(LIBDIR)/libMidi.a

$(LIBDIR)/libMidi.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libMidi.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libMidi.a

TOCLEAN = $(LIBDIR)/libMidi.a

include $(ROOTDIR)/Makefiles/Makefile.misc
