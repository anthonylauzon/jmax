# $Id: Makefile.am,v 1.14 2003/07/23 12:16:12 p_tisserand Exp $
#
# sequence package Makefile.am
#
# $Log: Makefile.am,v $
# Revision 1.14  2003/07/23 12:16:12  p_tisserand
# Changed installation directory of packages.
# Now packages are installed in ${libdir}/jmax/packages.
#
# Revision 1.13  2003/05/26 11:28:47  p_tisserand
# Added Visual C++ files in distribution.
#
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

SUBDIRS=c $(JAVA_SRC_DIR) help fonts images winbuild

packagedir=$(pkglibdir)/packages/sequence
package_DATA=sequence.jpkg

EXTRA_DIST= $(package_DATA)
