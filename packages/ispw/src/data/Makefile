#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:52:45 $
#
# Makefile for the New FTS classes library
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libdata.a $(LIBDIR)/libdata.so

$(LIBDIR)/libdata.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libdata.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libdata.a

TOCLEAN = $(LIBDIR)/libdata.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

