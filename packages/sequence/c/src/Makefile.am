# $Id: Makefile.am,v 1.14 2002/12/09 14:58:16 borghesi Exp $
#
# sequence package source Makefile.am
#
# $Log: Makefile.am,v $
# Revision 1.14  2002/12/09 14:58:16  borghesi
# fix on previous Norbert's commit
#
# Revision 1.13  2002/12/03 14:09:14  p_tisserand
# Add missing seqsym.h in noinst_HEADERS
#
# Revision 1.12  2002/12/02 15:38:17  diemo
# Avoid relative paths, because they don't work when configuring in a
# different directory, e.g.:
#
# $ cd buildir
# $ ../jmax/configure
# $ make
#
# Thanks to Patrice Tisserand:
# Les paths relatifs sont à éviter, cela ne fonctionne que pour les
# fichiers générés par configure lors que l'on compile dans un répertoire
# différent des sources.
#
# Revision 1.11  2002/12/02 12:10:04  diemo
# Fixed include scheme for internal/external headers
# (doc in c/src/Makefile.am)
# seqsym.h exported, too.


packagedir=$(pkgdatadir)/packages/sequence/c
package_LTLIBRARIES=libsequence.la
libsequence_la_SOURCES= \
event.c \
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
# There are five cases of #include:
# 
#
# From a package source or non-exported header file in this package:
#
# 1. a package source or non-exported header file includes its  
#     non-exported header 'local.h' in 'thispackage/c/src':
#
#    #include "local.h"	 -->  -I. is given by default
#
# 2. a package source or non-exported header file includes its 
#    exported header 'extern.h' in 'thispackage/c/include' 
#    (before it is installed!):
#
#    #include <extern.h>  -->  -I$(top_srcdir)/packages/mypackage/c/include
#    or
#    #include <sequence/c/include/extern.h>  -->  -I$(top_srcdir)/packages 
#
#
# From an exported header file in this package:
#
# 3. an exported header file includes another exported header 'extern.h' 
#    in 'thispackage/c/include' (before it is installed!):
#
#    #include <extern.h>  -->  -I$(top_srcdir)/packages/mypackage/c/include
#    
#    This is the only way it works for other packages that include this 
#    header!!!
#
#
# From a source or header file in another package:
#
# 4. a package in the jmax source-tree includes an exported header 'api.h'
#    in 'otherpackage/c/include' of another package which resides 
#    in the jmax source-tree (i.e. again before it is installed!):
#
#    #include <api.h>  -->  -I$(top_srcdir)/packages/otherpackage/c/include
#    or
#    #include <otherpackage/c/include/api.h>  -->  -I$(top_srcdir)/packages 
#
# 5. an external package (outside of the jmax source-tree) includes an
#    exported header 'api.h' of another package in
#    'installed-includes/fts/packages/otherpackage' (i.e. this time its
#    headers are already installed):
#
#    #include <fts/packages/otherpackage/api.h>  -->  includes already set
#

# package can include fts headers, other packages' headers, and its
# own exported headers from exported headers
INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/packages/sequence/c/include

noinst_HEADERS= \
seqmess.h \
seqmidi.h 

libsequence_la_LDFLAGS= -module
