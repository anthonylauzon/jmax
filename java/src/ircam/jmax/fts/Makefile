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
PACKAGE_DIR = ircam/jmax/fts

include $(ROOTDIR)/java/Makefiles/Makefile.common
include Sources

P_ROOT = ircam.jmax.fts

FTS_PACKAGES = $(P_ROOT).tcl $(P_ROOT).comm $(P_ROOT).dotpat

LOCAL_PACKAGES = $(FTS_PACKAGES) 

all:  local
	javac $(JFLAGS) LOCAL_PACKAGES
.PHONY: all

local: $(CLASS_FILES)

include $(ROOTDIR)/java/Makefiles/Makefile.misc
