#  $Id: Makefile.am,v 1.2 2002/08/30 17:20:57 schwarz Exp $
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
#
#  $Log: Makefile.am,v $
#  Revision 1.2  2002/08/30 17:20:57  schwarz
#  SDIF library now configured by configure script!
#
#  Revision 1.1  2002/08/30 14:07:13  schwarz
#  First working version of sdif package.
#  In/out of scalars (singleton matrices).
#  Definition of file-scope description types.
#  Compiles under jmax 2.5.3 and 3.
#
#  Revision 1.1  2002/08/12 13:12:14  schwarz
#  Initial revision

# all SDIF config is now set by:
# ./configure --prefix /u/formes/schwarz --bindir /u/formes/schwarz/bin/i686 --libdir /u/formes/schwarz/lib/i686 --enable-debug --with-sdif --with-sdif-incl=/u/formes/share/include --with-sdif-libdir=/u/formes/share/lib/$SYS

# defines and compilation options
BUILD_DATE	= $(shell date)
BUILD_ID	= '-DBUILD_DATE="$(BUILD_DATE) on $(HOST) for $(USER)"'
VERSION		= '-DSDIFVERSIONSTR="$(SDIFVERSION)"'

CFLAGS	       += $(BUILD_ID) $(VERSION)
INCLUDES       += -I$(top_srcdir)/include -I$(top_srcdir)/packages/data/c/src \
		  @SDIF_INCLUDE@


packagedir 		= $(pkgdatadir)/packages/sdif/c
# package lib to build
package_LTLIBRARIES	= libsdif.la

libsdif_la_CFLAGS	= -Wall
libsdif_la_SOURCES	= jmaxsdif.c readsdif.c writesdif.c
libsdif_la_LDFLAGS	= @SDIF_LIBDIR@ @SDIF_LIBNAME@

noinst_HEADERS	=
