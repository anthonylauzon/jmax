## Process this file with automake to produce Makefile.in :
## generate aclocal.m4
## aclocal
## generate libtool files
## libtoolize --automake -c
## automake --add-missing --copy --foreign
SUBDIRS=client fts include java packages bin tcl images tutorials config utils


#
# TAGS
# do the TAGS file
#
TAGS:
	find . \! \( -name '*~' -o \( -type d -name '*.old' -prune \) \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.scm" -o -name "*.tcl" -name "*.jpk" \) -print | etags -t -
.PHONY: TAGS

# This is for Ctags 4.0.2
NTAGS:
	find . \! \( -name '*~' \( -type d -name '*.old' -prune \) \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.scm" -o -name "*.tcl" -name "*.jpk" \) -print | etags -L -
.PHONY: NTAGS

