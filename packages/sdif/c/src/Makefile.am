#  $Id: Makefile.am,v 1.1 2002/08/12 13:12:14 schwarz Exp schwarz $
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
#  $Log: Makefile.am,v $
#  Revision 1.1  2002/08/12 13:12:14  schwarz
#  Initial revision


# where is the sdif library and sdif.h?  todo: configure
#ifeq ($(MODE), debug)
  debug_suffix	= -debug
#else
#  debug_suffix	=
#endif

sdif_include		= /u/formes/share/include
sdif_lib_dir		= /u/formes/share/lib/$(SYS)
sdif_lib_name		= libsdif$(debug_suffix).a

# defines and compilation options
BUILD_DATE	= $(shell date)
BUILD_ID	= '-DBUILD_DATE="$(BUILD_DATE) on $(HOST) for $(USER)"'
CFLAGS	       += $(BUILD_ID) '-DSDIFVERSIONSTR="$(SDIFVERSION)"'
INCLUDES       += -I$(top_srcdir)/include -I$(sdif_include)



packagedir 		= $(pkgdatadir)/packages/sdif/c
package_LTLIBRARIES	= libsdif.la

libsdif_la_CFLAGS	= -Wall
libsdif_la_SOURCES	= jmaxsdif.c readsdif.c writesdif.c
libsdif_la_LDFLAGS	= $(sdif_lib_dir)/$(sdif_lib_name)

noinst_HEADERS	=

