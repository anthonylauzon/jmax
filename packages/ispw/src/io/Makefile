#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.2 $ IRCAM $Date: 1998/06/17 15:44:42 $
#
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

ifeq ($(ARCH), solaris2)
MORESOLIBS=/opt/LOCAL/sol/lib/libST.so /usr/demo/SOUND/lib/libaudio.a
endif

all: $(LIBDIR)/libio.a $(LIBDIR)/libio.so

$(LIBDIR)/libio.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libio.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libio.a

TOCLEAN = $(LIBDIR)/libio.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

