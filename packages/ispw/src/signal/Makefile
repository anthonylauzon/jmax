#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:39 $
#
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libsignal.a $(LIBDIR)/libsignal.so

$(LIBDIR)/libsignal.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libsignal.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libsignal.a

TOCLEAN = $(LIBDIR)/libsignal.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

