# $Id$
#
# sequence package source Makefile.am
#
# $Log$

packagedir=$(pkgdatadir)/packages/sequence/c
package_LTLIBRARIES=libsequence.la
libsequence_la_SOURCES= \
event.c \
getdur.c \
locate.c \
module.c \
note.c \
seqmess.c \
seqfind.c \
seqmidi.c \
seqplay.c \
seqrec.c \
seqstep.c \
seqsym.c \
sequence.c \
track.c


# HOW TO INCLUDE:
#
# there are five cases of #include:
# 
#
# From a package source or non-exported header file in this package:
#
# 1. a package source or non-exported header file includes its  
#     non-exported header 'local.h' in 'thispackage/c/src':
#
#    #include "local.h"
#    --> -I. is given by default
#
# 2. a package source or non-exported header file includes its 
#    exported header 'extern.h' in 'thispackage/c/include' 
#    (before it is installed!):
#
#    #include <extern.h>  or  #include <sequence/c/include/extern.h>
#    --> -I../include     or  -I$(top_srcdir)/packages 
#
#
# From an exported header file in this package:
#
# 3. an exported header file includes another exported header 'extern.h' 
#    in 'thispackage/c/include' (before it is installed!):
#
#    #include <extern.h>  is the only way it works for other packages 
#			  that include this header!!!
#    --> -I../include
#
#
# From a source or header file in another package:
#
# 4. a package in the jmax source-tree includes an exported header 'api.h'
#    in 'otherpackage/c/include' of another package which resides 
#    in the jmax source-tree (i.e. again before it is installed!):
#
#    #include <api.h>     or  #include <otherpackage/c/include/api.h>
#    --> -I$(top_srcdir)/packages/otherpackage/c/include
#                         or  -I$(top_srcdir)/packages 
#
# 5. an external package (outside of the jmax source-tree) includes an exported
#    header 'api.h' of another package in 'installed-include/fts/otherpackage'
#    (i.e. this time its headers are already installed):
#
#    #include <fts/otherpackage/api.h>
#    --> includes already set
#

# package can include fts headers, other packages' headers, and its
# own exported headers from exported headers
INCLUDES = -I$(top_srcdir)/include -I../include

noinst_HEADERS= \
seqmess.h \
seqmidi.h

libsequence_la_LDFLAGS= -module
