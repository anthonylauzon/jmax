#
#			Copyright (c) 1995 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
# $Revision: 1.8 $ IRCAM $Date: 1996/10/30 14:31:54 $
#
# Makefile for the New FTS message library
#

ROOTDIR = ../../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

all: $(LIBDIR)/libFtl.a

$(LIBDIR)/libFtl.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libFtl.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libFtl.a

TOCLEAN = $(LIBDIR)/libFtl.a

include $(ROOTDIR)/Makefiles/Makefile.misc
