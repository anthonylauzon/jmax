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


ifdef STATIC
FTS_BIN_STATIC= $(BINDIR)/fts.static

FTS_PACKAGE_DIR=$(ROOTDIR)/../packages

FTS_PACKAGE_LIBS=\
$(FTS_PACKAGE_DIR)/additive/lib/$(ARCH)/$(MODE)/libadditive.a \
$(FTS_PACKAGE_DIR)/eric/lib/$(ARCH)/$(MODE)/liberic.a \
$(FTS_PACKAGE_DIR)/explode/c/lib/$(ARCH)/$(MODE)/libexplode.a \
$(FTS_PACKAGE_DIR)/guiobj/c/lib/$(ARCH)/$(MODE)/libguiobj.a \
$(FTS_PACKAGE_DIR)/ispw/lib/$(ARCH)/$(MODE)/libcontrol.a \
$(FTS_PACKAGE_DIR)/ispw/lib/$(ARCH)/$(MODE)/libdata.a \
$(FTS_PACKAGE_DIR)/ispw/lib/$(ARCH)/$(MODE)/libfilters.a \
$(FTS_PACKAGE_DIR)/ispw/lib/$(ARCH)/$(MODE)/libio.a \
$(FTS_PACKAGE_DIR)/ispw/lib/$(ARCH)/$(MODE)/libmidi.a \
$(FTS_PACKAGE_DIR)/ispw/lib/$(ARCH)/$(MODE)/libpitch.a \
$(FTS_PACKAGE_DIR)/ispw/lib/$(ARCH)/$(MODE)/libsampling.a \
$(FTS_PACKAGE_DIR)/ispw/lib/$(ARCH)/$(MODE)/libsignal.a \
$(FTS_PACKAGE_DIR)/lists/lib/$(ARCH)/$(MODE)/liblists.a \
$(FTS_PACKAGE_DIR)/manoury/lib/$(ARCH)/$(MODE)/libmanoury.a \
$(FTS_PACKAGE_DIR)/qlist/c/lib/$(ARCH)/$(MODE)/libqlist.a \
$(FTS_PACKAGE_DIR)/system/c/lib/$(ARCH)/$(MODE)/libsystem.a

all: $(FTS_BIN_STATIC)

$(FTS_BIN_STATIC) : $(OBJECTS) $(FTS_LIBS) $(ARCH_LIBS) $(FTS_ARCH_LIBS)
	rm -f $(FTS_BIN_STATIC)
	$(LD) $(FTS_LDFLAGS) -o $@ $(OBJECTS) $(CC_ALL_FLAG) $(FTS_PACKAGE_LIBS) $(FTS_LIBS) $(CC_NONE_FLAG) $(FTS_ARCH_LIBS) $(FTS_SYS_LIBS) $(FTS_USER_LIBS_FLAGS) $(FTS_USER_LIBS) 

else

FTS_BIN = $(BINDIR)/fts

all: $(FTS_BIN)

endif



TOCLEAN = $(FTS_BIN)

include $(ROOTDIR)/Makefiles/Makefile.misc
