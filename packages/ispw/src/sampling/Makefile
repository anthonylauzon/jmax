#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:21 $
#
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libsampling.a $(LIBDIR)/libsampling.so

$(LIBDIR)/libsampling.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libsampling.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libsampling.a

TOCLEAN = $(LIBDIR)/libsampling.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

