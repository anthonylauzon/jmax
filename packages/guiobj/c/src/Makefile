#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:56 $
#
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libguiobj.a $(LIBDIR)/libguiobj.so

$(LIBDIR)/libguiobj.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libguiobj.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libguiobj.a

TOCLEAN = $(LIBDIR)/libguiobj.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

