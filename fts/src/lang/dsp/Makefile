#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.10 $ IRCAM $Date: 1996/10/30 14:34:44 $
#
# Makefile for the New FTS message library
#

ROOTDIR = ../../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

all: $(LIBDIR)/libDsp.a

$(LIBDIR)/libDsp.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libDsp.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libDsp.a

TOCLEAN = $(LIBDIR)/libDsp.a

include $(ROOTDIR)/Makefiles/Makefile.misc
