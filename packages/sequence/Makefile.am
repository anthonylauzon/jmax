# $Id: Makefile.am,v 1.12 2002/12/02 12:10:02 diemo Exp $
#
# sequence package Makefile.am
#
# $Log: Makefile.am,v $
# Revision 1.12  2002/12/02 12:10:02  diemo
# Fixed include scheme for internal/external headers
# (doc in c/src/Makefile.am)
# seqsym.h exported, too.
#

if COND_COMPILE_JAVA
JAVA_SRC_DIR= java
else
JAVA_SRC_DIR= 
endif

SUBDIRS=c $(JAVA_SRC_DIR) help images winbuild

packagedir=$(pkgdatadir)/packages/sequence
package_DATA=sequence.jpkg

EXTRA_DIST= $(package_DATA)
