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


ROOTDIR = ../../../../..
PACKAGE_DIR = ircam/jmax/editors

include $(ROOTDIR)/java/Makefiles/Makefile.common

P_ROOT = ircam.jmax

ERMES_PACAKGES = $(P_ROOT).editors.ermes.tcl \
                 $(P_ROOT).editors.ermes.connections \
                 $(P_ROOT).editors.ermes.connections.tcl \
                 $(P_ROOT).editors.ermes.objects \
                 $(P_ROOT).editors.ermes.objects.tcl

CONSOLE_PACKAGES = $(P_ROOT).editors.console
PROJECT_PACKAGES = $(P_ROOT).editors.project

EDITORS_PACKAGES = $(ERMES_PACKAGES) $(CONSOLE_PACKAGES) $(PROJECT_PACKAGES)

LOCAL_PACKAGES = $(EDITORS_PACKAGES)


all: 
	javac $(JFLAGS) $(LOCAL_PACKAGES)
.PHONY: all

include $(ROOTDIR)/java/Makefiles/Makefile.misc


