#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1998/09/18 11:56:50 $
#
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libexplode.a $(LIBDIR)/libexplode.so

$(LIBDIR)/libexplode.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libexplode.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libexplode.a

TOCLEAN = $(LIBDIR)/libexplode.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

