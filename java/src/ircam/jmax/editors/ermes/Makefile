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


ROOTDIR = ../../../../../..
PACKAGE_DIR = ircam/jmax/editors/ermes

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

ERMES_PACAKGES = $(P_ROOT).editors.ermes \
                 $(P_ROOT).editors.ermes.tcl \
                 $(P_ROOT).editors.ermes.connections \
                 $(P_ROOT).editors.ermes.connections.tcl \
                 $(P_ROOT).editors.ermes.objects \
                 $(P_ROOT).editors.ermes.objects.tcl

all:  local
	javac $(JFLAGS) $(LOCAL_PACKAGES)
.PHONY: all

local: $(CLASS_FILES)

include $(ROOTDIR)/java/Makefiles/Makefile.misc
