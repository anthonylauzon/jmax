#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:53:03 $
#
#

DISTDIR = ../../../fts
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/liblists.a $(LIBDIR)/liblists.so

$(LIBDIR)/liblists.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/liblists.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/liblists.a

TOCLEAN = $(LIBDIR)/liblists.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

