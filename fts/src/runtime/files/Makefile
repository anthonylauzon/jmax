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

all: $(LIBDIR)/libFiles.a

$(LIBDIR)/libFiles.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libFiles.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libFiles.a

TOCLEAN = $(LIBDIR)/libFiles.a

include $(ROOTDIR)/Makefiles/Makefile.misc
