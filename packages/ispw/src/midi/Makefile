#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:16 $
#
#

DISTDIR = ../../../../fts/
PACKAGEROOT = ../..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libmidi.a $(LIBDIR)/libmidi.so

$(LIBDIR)/libmidi.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libmidi.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libmidi.a

TOCLEAN = $(LIBDIR)/libmidi.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

