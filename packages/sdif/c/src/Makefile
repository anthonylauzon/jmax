#  $Id$
#
#  sdif package Makefile for jMax 2.5.x		August 2002	Diemo Schwarz
#
#  jMax
#  
#  Copyright (C) 2002 by IRCAM
#  All rights reserved.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
#  $Log$


# some hardcoded values
ifeq ($(USER), schwarz)
	JMAXDISTDIR	= /u/formes/schwarz/src/jmax-2.5.3
	ARCH		= i686-linux
	MODE		= debug
else
ifeq ($(HOST), lipari.ircam.fr)
        JMAXDISTDIR     = /usr/src/redhat/BUILD/jmax-2.5.3
	ARCH		= i686-linux
# no debug-compiled fts 
	MODE		= opt
else
#	default
	JMAXDISTDIR	= $(PROJ_ROOT)/../../../jmax
endif
endif

include $(JMAXDISTDIR)/Makefiles/Makefile.$(ARCH)


PACKAGEROOT	= ..
BUILD_DATE	= $(shell date)
BUILD_ID	= '-DBUILD_DATE="$(BUILD_DATE) on $(HOST) for $(USER)"'


# where to find the sdif library
ifeq ($(MODE), debug)
  debug_suffix	= -debug
else
  debug_suffix	=
endif

sdif_include	= /u/formes/share/include
sdif_lib_dir	= /u/formes/share/lib/$(SYS)
sdif_lib_name	= libsdif$(debug_suffix).a


# how to build
MODULE_CFLAGS	= -I$(sdif_include) $(BUILD_ID) \
		  '-DSDIFVERSIONSTR="$(SDIFVERSION)"'
MORESOLIBS	= $(sdif_lib_dir)/$(sdif_lib_name)


# what to build
PACKAGELIB	= $(LIBDIR)/libsdif.so


include $(JMAXDISTDIR)/Makefiles/Makefile.package


db:
	$(MAKE) ARCH=i686-linux MODE=debug 
opt:
	$(MAKE) ARCH=i686-linux

tags:
	etags -V *.[ch]

dist:	$(PACKAGELIB) .FORCE
	tar cvfzh jmaxsdif.tar.gz -C ../../.. sdif/c/lib sdif/sdif.jpk \
				              sdif/c/test/readwritesuivi.jmax \
				  -C $(HOME) suivi/sdiftypes_suiviaudio.styp

.FORCE:
.PHONY: FORCE
