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

JMAXDISTDIR=.

ifdef ARCH
include $(JMAXDISTDIR)/Makefiles/Makefile.$(ARCH)
endif

all:
	$(MAKE) -C fts $@
	$(MAKE) -C java $@
	$(MAKE) -C packages $@
.PHONY: all

all_c:
	$(MAKE) -C fts all
	$(MAKE) -C packages all_c
.PHONY: all_c

all_java:
	$(MAKE) -C java all
	$(MAKE) -C packages all_java
.PHONY: all_java

clean:
	$(MAKE) -C fts $@
	$(MAKE) -C java $@
	$(MAKE) -C packages $@
.PHONY: clean

clean_c:
	$(MAKE) -C fts clean
	$(MAKE) -C packages clean_c
.PHONY: clean_c

clean_java:
	$(MAKE) -C java clean
	$(MAKE) -C packages clean_java
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
	find . \! \( -name '*~' \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.scm" -o -name "*.tcl" -name "*.jpk" \) -print | etags -t -
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
		TTT=V`cut -f1 -d\  VERSION | sed 's/\./_/g'` ; \
		echo "Tagging with tag $$TTT" ; \
		cvs tag -F $$TTT ; \
	)
.PHONY: cvs-tag

#
# spec_files
# update the spec files for version number
#
spec-files:
	$(MAKE) -C utils/rpm $@
	$(MAKE) -C utils/sgi $@
.PHONY: spec-files

#
# dist
# does a cvs export and a .tar.gz of the sources
#
dist:
	( \
		umask 22 ; \
		VVV=`cut -f1 -d\  VERSION` ; \
		TTT=V`cut -f1 -d\  VERSION | sed 's/\./_/g'` ; \
		mkdir .$$$$ ; \
		cd .$$$$ ; \
		cvs export -r$$TTT jmax ; \
		mv jmax jmax-$${VVV} ; \
		tar cvf - jmax-$${VVV} | gzip -c --best > ../../jmax-$${VVV}-src.tar.gz ; \
		chmod 644 ../../jmax-$${VVV}-src.tar.gz ; \
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

install-doc:
	$(INSTALL_DIR) $(doc_install_dir)
	$(INSTALL_DATA) LICENCE.fr $(doc_install_dir)/LICENCE.fr
	$(INSTALL_DATA) LICENSE $(doc_install_dir)/LICENSE
	$(INSTALL_DATA) README $(doc_install_dir)/README
	$(INSTALL_DATA) VERSION $(doc_install_dir)/VERSION
	$(MAKE) -C doc doc_install_dir=$(doc_install_dir) install
.PHONY: install-doc

install-bin:
	$(INSTALL_DIR) $(bin_install_dir)
	$(MAKE) -C bin install
	$(INSTALL_DIR) $(lib_install_dir)
	$(MAKE) -C config install 
	$(MAKE) -C images install 
	$(MAKE) -C java install 
	$(MAKE) -C scm install 
	$(MAKE) -C tcl install 
	$(MAKE) -C fts install
	$(MAKE) -C packages install 
# The following line is a hack that installs the <package>.jpk file on all platforms
# (this is so that you can load the sgidev package even if you are running the GUI
# on Linux
	$(MAKE) -C packages install-noarch 
.PHONY: install-bin

install-includes:
	$(MAKE) -C include install
.PHONY: install-includes


#
# new-patch, new-minor, new-major
# version number manipulation ('make new-patch' is the most frequent...)
#
new-patch:
	awk '{ split( $$1, a, "."); printf( "%d.%d.%d\n", a[1], a[2], a[3]+1) }' VERSION > VERSION.out
	mv VERSION.out VERSION
	$(MAKE) spec-files

new-minor:
	awk '{ split( $$1, a, "."); printf( "%d.%d.%d\n", a[1], a[2]+1, 0) }'  VERSION > VERSION.out
	mv VERSION.out VERSION
	$(MAKE) spec-files

new-major:
	awk '{ split( $$1, a, "."); printf( "%d.%d.%d\n", a[1]+1, 0, 0) }'  VERSION > VERSION.out
	mv VERSION.out VERSION
	$(MAKE) spec-files
