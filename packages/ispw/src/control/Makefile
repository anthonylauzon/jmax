#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:52:33 $
#
# Makefile for the New FTS classes library
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libcontrol.a $(LIBDIR)/libcontrol.so

$(LIBDIR)/libcontrol.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libcontrol.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libcontrol.a

TOCLEAN = $(LIBDIR)/libcontrol.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

