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

all: $(LIBDIR)/libMess.a

$(LIBDIR)/libMess.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libMess.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libMess.a

TOCLEAN = $(LIBDIR)/libMess.a

include $(ROOTDIR)/Makefiles/Makefile.misc
