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


ROOTDIR = ../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources


all: $(LIBDIR)/libSys.a

$(LIBDIR)/libSys.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libSys.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libSys.a

TOCLEAN = $(LIBDIR)/libSys.a

include $(ROOTDIR)/Makefiles/Makefile.misc
