#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:52:53 $
#
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libfilters.a $(LIBDIR)/libfilters.so

$(LIBDIR)/libfilters.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libfilters.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libfilters.a

TOCLEAN = $(LIBDIR)/libfilters.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

