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


ROOTDIR = ../../../..
PACKAGE_DIR = ircam/jmax

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

P_ROOT = ircam.jmax
FTS_PACKAGES = $(P_ROOT).fts $(P_ROOT).fts.tcl $(P_ROOT).fts.comm $(P_ROOT).fts.dotpat
TCL_PACKAGES = $(P_ROOT).tcl
DIALOGS_PACKAGES = $(P_ROOT).dialogs
UTILS_PACKAGES = $(P_ROOT).utils

ERMES_PACAKGES = $(P_ROOT).editors.ermes \
                 $(P_ROOT).editors.ermes.tcl \
                 $(P_ROOT).editors.ermes.connections \
                 $(P_ROOT).editors.ermes.connections.tcl \
                 $(P_ROOT).editors.ermes.objects \
                 $(P_ROOT).editors.ermes.objects.tcl

CONSOLE_PACKAGES = $(P_ROOT).editors.console
PROJECT_PACKAGES = $(P_ROOT).editors.project

EDITORS_PACKAGES = $(ERMES_PACKAGES) $(CONSOLE_PACKAGES) $(PROJECT_PACKAGES)

LOCAL_PACKAGES = $(FTS_PACKAGES)  $(TCL_PACKAGES) $(DIALOGS_PACKAGES) $(UTILS_PACKAGES) $(EDITORS_PACKAGES)

all:  local
	javac $(JFLAGS) $(LOCAL_PACKAGES)
.PHONY: all

local: $(CLASS_FILES)

include $(ROOTDIR)/java/Makefiles/Makefile.misc
