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

all: $(LIBDIR)/libSched.a

$(LIBDIR)/libSched.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libSched.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libSched.a

TOCLEAN = $(LIBDIR)/libSched.a

include $(ROOTDIR)/Makefiles/Makefile.misc
