#
#                      FTS 2.x makefile for GNU Make
#
#			Copyright (c) 1997 by IRCAM
#			    All rights reserved.
#
#  For any information regarding this and other IRCAM software, please
#  send email to:
#                              manager@ircam.fr
#
#
#


ROOTDIR = ../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

DATE:= $(shell date)
MODULE_CFLAGS:= -DCOMPILATION_INFO_STRING='"Compiled by $(USER) for $(ARCH) on $(DATE)\n"'

FTS_BIN = $(BINDIR)/fts

ifeq ($(ARCH),rhapsody)

FTS_PACKAGE_LIBDIR=$(ROOTDIR)/../packages/fts1.5/lib/$(ARCH)/$(MODE)
FTS_PACKAGE_LIBS= \
$(FTS_PACKAGE_LIBDIR)/libadditive.a \
$(FTS_PACKAGE_LIBDIR)/libcontrol.a \
$(FTS_PACKAGE_LIBDIR)/libdata.a \
$(FTS_PACKAGE_LIBDIR)/libevents.a \
$(FTS_PACKAGE_LIBDIR)/libfilters.a \
$(FTS_PACKAGE_LIBDIR)/libio.a \
$(FTS_PACKAGE_LIBDIR)/liblists.a \
$(FTS_PACKAGE_LIBDIR)/libmath.a \
$(FTS_PACKAGE_LIBDIR)/libmidi.a \
$(FTS_PACKAGE_LIBDIR)/libsampling.a \
$(FTS_PACKAGE_LIBDIR)/libscorefollow.a \
$(FTS_PACKAGE_LIBDIR)/libsigbasic.a \
$(FTS_PACKAGE_LIBDIR)/libsigmath.a \
$(FTS_PACKAGE_LIBDIR)/libsystem.a \
$(FTS_PACKAGE_LIBDIR)/libui.a 

endif

all: $(FTS_BIN)

ifeq ($(ARCH),rhapsody)

$(FTS_BIN) : $(OBJECTS) $(FTS_LIBS) $(ARCH_LIBS) $(FTS_ARCH_LIBS)
	rm -f $(FTS_BIN)
	$(LD) $(FTS_LDFLAGS) -o $@ $(OBJECTS) $(CC_ALL_FLAG)  $(EXTRA_LDFLAGS) $(FTS_PACKAGE_LIBS) $(FTS_LIBS) $(CC_NONE_FLAG) $(FTS_ARCH_LIBS) $(FTS_SYS_LIBS) $(FTS_USER_LIBS_FLAGS)  $(FTS_USER_LIBS) 

endif

TOCLEAN = $(FTS_BIN)

include $(ROOTDIR)/Makefiles/Makefile.misc
