#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:31 $
#
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libpitch.a $(LIBDIR)/libpitch.so

$(LIBDIR)/libpitch.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libpitch.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libpitch.a

TOCLEAN = $(LIBDIR)/libpitch.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

