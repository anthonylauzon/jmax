# 
# jMax
# Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# See file LICENSE for further informations on licensing terms.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# Based on Max/ISPW by Miller Puckette.
#
# Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#

DISTDIR=.

ifdef ARCH
include $(DISTDIR)/Makefiles/Makefile.$(ARCH)
endif

all:
	(cd fts; $(MAKE) $@)
	(cd java ; $(MAKE) $@)
	(cd lib; $(MAKE) $@)
	(cd packages; $(MAKE) $@)
.PHONY: all

all_c:
	(cd fts; $(MAKE) all)
	(cd lib; $(MAKE) all_c)
	(cd packages; $(MAKE) all_c)
.PHONY: all_c

all_java:
	(cd java ; $(MAKE) all)
	(cd lib; $(MAKE) all_java)
	(cd packages; $(MAKE) all_java)
.PHONY: all_java

clean:
	(cd fts; $(MAKE) $@)
	(cd java ; $(MAKE) $@)
	(cd lib; $(MAKE) $@)
	(cd packages; $(MAKE) $@)
.PHONY: clean

clean_c:
	(cd fts; $(MAKE) clean)
	(cd packages; $(MAKE) clean_c)
	(cd lib; $(MAKE) clean_c)
.PHONY: clean_c

clean_java:
	(cd java ; $(MAKE) clean)
	(cd lib; $(MAKE) clean_java)
	(cd packages; $(MAKE) clean_java)
.PHONY: clean_java

#
# i386-linux
# target for Linux/i386
#
i386-linux:
	$(MAKE) ARCH=i386-linux all
.PHONY: i386-linux

#
# i686-linux
# target for Linux/PentiumPro-PentiumIIx
#
i686-linux:
	$(MAKE) ARCH=i686-linux all
.PHONY: i686-linux

#
# ppc-linux
# target for Linux/PowerPC
#
ppc-linux:
	$(MAKE) ARCH=ppc-linux all
.PHONY: ppc-linux

#
# ppc-macosx
# target for Mac OS/X on PowerPC
#
ppc-macosx:
	$(MAKE) ARCH=ppc-macosx all
.PHONY: ppc-macosx

#
# r5k-irix6.5
# target for Irix 6.5 on R5000 processor
#
r5k-irix6.5:
	$(MAKE) ARCH=r5k-irix6.5 all
.PHONY: r5k-irix6.5

#
# r10k-irix6.5
# target for Irix 6.5 on R10000 processor
#
r10k-irix6.5:
	$(MAKE) ARCH=r10k-irix6.5 all
.PHONY: r10k-irix6.5

#
# TAGS
# do the TAGS file
#
TAGS:
	find . \! \( -name '*~' \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.scm" -o -name "*.html" \) -print | etags -t -
.PHONY: TAGS

#
# cvs-tag
#
cvs-tag: spec-files
	if cvs -n update 2>&1 | egrep '^[ACMPRU] ' ; then \
		echo "Not sync with cvs (do an update or commit)" ; \
		exit 1 ; \
	fi
	( \
		TTT=V`sed 's/\./_/g' VERSION` ; \
		echo "Tagging with tag $$TTT" ; \
		cvs tag -F $$TTT ; \
	)
.PHONY: cvs-tag

#
# spec_files
# update the spec files for version number
#
spec-files:
	(cd utils/rpm ; $(MAKE) $@)
	(cd utils/sgi ; $(MAKE) $@)
.PHONY: spec-files

#
# dist
# does a cvs export and a .tar.gz of the sources
#
dist:
	( \
		umask 22 ; \
		mkdir .$$$$ ; \
		cd .$$$$ ; \
		VVV=`cat ../VERSION` ; \
		TTT=V`sed 's/\./_/g' ../VERSION` ; \
		cvs export -r$$TTT jmax ; \
		mv jmax jmax-$$VVV ; \
		tar cvf - jmax-$$VVV | gzip -c --best > ../../jmax-$$VVV-src.tar.gz ; \
		chmod 644 ../../jmax-$$VVV-src.tar.gz ; \
		cd .. ; \
		/bin/rm -rf .$$$$ ; \
	)
.PHONY: dist

old-dist:
	( \
		umask 22 ; \
		mkdir .$$$$ ; \
		cd .$$$$ ; \
		TTT=V`echo $(VERSION) | sed 's/\./_/g'` ; \
		cvs export -r$$TTT jmax ; \
		mv jmax jmax-$(VERSION) ; \
		tar cvf - jmax-$(VERSION) | gzip -c --best > ../../jmax-$(VERSION)-src.tar.gz ; \
		chmod 644 ../../jmax-$(VERSION)-src.tar.gz ; \
		cd .. ; \
		/bin/rm -rf .$$$$ ; \
	)
.PHONY: old-dist


#
# install
# copies the files to the right directories
#
install: install-doc install-bin install-includes
.PHONY: install

MAKE_INSTALL=$(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" INSTALL_LIB="$(INSTALL_LIB)" INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_SETUID="$(INSTALL_SETUID)"

install-doc:
	$(INSTALL_DIR) $(doc_install_dir)
	$(INSTALL_DATA) LICENCE.fr LICENSE README VERSION $(doc_install_dir)
	( cd doc ; $(MAKE_INSTALL) doc_install_dir=$(doc_install_dir) install )
.PHONY: install-doc

install-bin:
	$(INSTALL_DIR) $(bin_install_dir)
	( cd bin ; $(MAKE_INSTALL) bin_install_dir=$(bin_install_dir) install )
	$(INSTALL_DIR) $(lib_install_dir)
	( cd config ; $(MAKE_INSTALL) lib_install_dir=$(lib_install_dir) install )
	( cd images ; $(MAKE_INSTALL) lib_install_dir=$(lib_install_dir) install )
	( cd java ; $(MAKE_INSTALL) lib_install_dir=$(lib_install_dir) install )
	( cd scm ; $(MAKE_INSTALL) lib_install_dir=$(lib_install_dir) install )
	( cd tcl ; $(MAKE_INSTALL) lib_install_dir=$(lib_install_dir) install )
	( cd fts ; $(MAKE_INSTALL) lib_install_dir=$(lib_install_dir) install ) ; \
	( cd packages ; $(MAKE_INSTALL) lib_install_dir=$(lib_install_dir) install )
.PHONY: install-bin

install-includes:
	( cd fts/src ; $(MAKE_INSTALL) include_install_dir=$(include_install_dir) install )
.PHONY: install-includes

#
# new-patch, new-minor, new-major
# version number manipulation ('make new-patch' is the most frequent...)
#
new-patch:
	awk -F. '{printf( "%d.%d.%d\n", $1, $2, $3+1);}' VERSION
	$(MAKE) spec-files

new-minor:
	awk -F. '{printf( "%d.%d.%d\n", $1, $2+1, 0);}' VERSION
	$(MAKE) spec-files

new-major:
	awk -F. '{printf( "%d.%d.%d\n", $1+1, 0, 0);}' VERSION
	$(MAKE) spec-files
