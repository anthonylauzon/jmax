## Process this file with automake to produce Makefile.in :
## generate aclocal.m4
## aclocal
## generate libtool files
## libtoolize --automake -c
## automake --add-missing --copy --foreign
SUBDIRS=client fts include java packages bin images tutorials config utils


#
# TAGS
# do the TAGS file
#
TAGS:
	find . \! \( -name '*~' -o \( -type d -name '*.old' -prune \) \) \( -name "*.c" -o -name "*.c.in" -o -name "*.h" -o -name "*.h.in" -o -name "*.java" -o -name "*.java.in" -o -name "Makefile.*" -o -name "Makefile" -o -o  -name "*.jpk" \) -print | etags -t -
.PHONY: TAGS

# This is for Ctags 4.0.2
NTAGS:
	find . \! \( -name '*~' -o \( -type d -name '*.old' -prune \) \) \( -name "*.c" -o -name "*.c.in" -o -name "*.h" -o -name "*.h.in" -o -name "*.java" -o -name "*.java.in" -o -name "Makefile.*" -o -name "Makefile" -o  -name "*.jpk" \) -print | etags -L -
.PHONY: NTAGS


#
# new-patch, new-minor, new-major
# version number manipulation (new-patch is the most frequent, new-major the least...)
#
SNAPSHOT_STATUS=_latest_cvs

new-patch:
	(\
		. ./JMAX-VERSION ; \
		echo JMAX_MAJOR_VERSION=$$JMAX_MAJOR_VERSION ; \
		echo JMAX_MINOR_VERSION=$$JMAX_MINOR_VERSION ; \
		echo JMAX_PATCH_VERSION=`expr $$JMAX_PATCH_VERSION + 1`; \
		echo JMAX_VERSION_STATUS=$(SNAPSHOT_STATUS); \
	) > JMAX-VERSION.out
	mv JMAX-VERSION.out JMAX-VERSION

new-minor:
	(\
		. ./JMAX-VERSION ; \
		echo JMAX_MAJOR_VERSION=$$JMAX_MAJOR_VERSION ; \
		echo JMAX_MINOR_VERSION=`expr $$JMAX_MINOR_VERSION + 1` ; \
		echo JMAX_PATCH_VERSION=0 ; \
		echo JMAX_VERSION_STATUS=$(SNAPSHOT_STATUS); \
	) > JMAX-VERSION.out
	mv JMAX-VERSION.out JMAX-VERSION

new-major:
	(\
		. ./JMAX-VERSION ; \
		echo JMAX_MAJOR_VERSION=`expr $$JMAX_MAJOR_VERSION + 1` ; \
		echo JMAX_MINOR_VERSION=0 ; \
		echo JMAX_PATCH_VERSION=0 ; \
		echo JMAX_VERSION_STATUS=$(SNAPSHOT_STATUS); \
	) > JMAX-VERSION.out
	mv JMAX-VERSION.out JMAX-VERSION

final:
	(\
		. ./JMAX-VERSION ; \
		echo JMAX_MAJOR_VERSION=$$JMAX_MAJOR_VERSION ; \
		echo JMAX_MINOR_VERSION=$$JMAX_MINOR_VERSION ; \
		echo JMAX_PATCH_VERSION=$$JMAX_PATCH_VERSION ; \
		echo JMAX_VERSION_STATUS=; \
	) > JMAX-VERSION.out
	mv JMAX-VERSION.out JMAX-VERSION
