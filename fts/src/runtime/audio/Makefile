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

all: $(LIBDIR)/libAudio.a

$(LIBDIR)/libAudio.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libAudio.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libAudio.a

TOCLEAN = $(LIBDIR)/libAudio.a

include $(ROOTDIR)/Makefiles/Makefile.misc
