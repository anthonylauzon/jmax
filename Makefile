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

include VERSION

ifdef SNAPSHOT
version=$(MAJOR).$(MINOR).$(PATCH_LEVEL)$(SNAPSHOT)
disttag=V$(MAJOR)_$(MINOR)_$(PATCH_LEVEL)$(SNAPSHOT)
else
version=$(MAJOR).$(MINOR).$(PATCH_LEVEL)
disttag=V$(MAJOR)_$(MINOR)_$(PATCH_LEVEL)
endif
distdir=jmax-$(version)


ifeq ($(ARCH),sgi)
INSTALL=install
INSTALL_DATA=$(INSTALL) -m 644
INSTALL_PROGRAM=$(INSTALL) -m 755
INSTALL_SETUID=$(INSTALL) -m 4755
INSTALL_LIB=$(INSTALL) -m 755
INSTALL_DIR=$(INSTALL) -d -m 755
prefix=/usr
doc_install_dir=$(prefix)/webdocs/jMax
lib_install_dir=$(prefix)/lib/jmax
include_install_dir=$(prefix)/include
bin_install_dir=$(prefix)/bin
SUB_ARCHS=irix65r10k irix65r5k
else
ifeq ($(ARCH),linuxpc)
INSTALL=install
INSTALL_DATA=$(INSTALL) --mode=0644
INSTALL_PROGRAM=$(INSTALL) --mode=0755
INSTALL_SETUID=$(INSTALL) --mode=4755
INSTALL_LIB=$(INSTALL) --mode=0755
INSTALL_DIR=$(INSTALL) -d --mode=0755
prefix=/usr
doc_install_dir=$(prefix)/doc/jMax-$(version)
lib_install_dir=$(prefix)/lib/jmax
include_install_dir=$(prefix)/include
bin_install_dir=$(prefix)/bin
SUB_ARCHS=linuxpc
else
endif
endif

all:
	(cd fts; $(MAKE) all)
	(cd java ; $(MAKE) all)
	(cd lib; $(MAKE) all)
	(cd packages; $(MAKE) all)
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
	(cd fts; $(MAKE) clean)
	(cd java ; $(MAKE) clean)
	(cd lib; $(MAKE) clean)
	(cd packages; $(MAKE) clean)
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
# linuxpc
# target for Linux/i386
#
linuxpc:
	$(MAKE) ARCH=linuxpc all
.PHONY: linuxpc

#
# sgi
# target for SGI Irix 6.5 processors R5000 and R10000
#
sgi:
	$(MAKE) ARCH=irix65r5k all
	$(MAKE) ARCH=irix65r10k all
.PHONY: sgi

#
# TAGS
# do the TAGS file
#
TAGS:
	find . \! \( -name '*~' \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.tcl" -o -name "*.html" \) -print | etags -t -
.PHONY: TAGS

#
# cvs-tag
#
cvs-tag:
	if cvs -n update 2>&1 | egrep -s '^[UARMC] ' ; then \
		echo "not sync with cvs (update or commit)" ; exit 1 \
	fi
	cvs tag -F $(disttag)
.PHONY: cvs-tag

#
# dist
# does a cvs export and a .tar.gz of the sources
#
dist: cvs-tag
	rm -rf $(distdir)
	mkdir $(distdir)
	cvs export -r $(disttag) -d $(distdir) max
	tar cvf - $(distdir) | gzip -c --best > $(distdir).tar.gz 
	chmod 644 $(distdir).tar.gz 
	rm -rf $(distdir)
.PHONY: dist

#
# install
# copies the files to the right directories
#
install: install-exec install-doc install-includes
.PHONY: install

install-doc:
	$(INSTALL_DIR) $(doc_install_dir)
	$(INSTALL_DATA) LICENCE.fr $(doc_install_dir)
	$(INSTALL_DATA) LICENSE $(doc_install_dir)
	$(INSTALL_DATA) README $(doc_install_dir)
	$(INSTALL_DATA) VERSION $(doc_install_dir)
	( cd doc ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" doc_install_dir=$(doc_install_dir) $@ )
.PHONY: install-doc

install-exec:
	( cd bin ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DIR="$(INSTALL_DIR)" bin_install_dir=$(bin_install_dir) $@ )
	$(INSTALL_DIR) $(lib_install_dir)
	( cd config ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) $@ )
	( cd fts ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_SETUID="$(INSTALL_SETUID)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) $@ )
	( cd images ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) $@ )
	( cd java ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) $@ )
	( cd scm ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) $@ )
	( cd tcl ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) $@ )
	( cd packages ; $(MAKE) INSTALL_LIB="$(INSTALL_LIB)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) SUB_ARCHS="$(SUB_ARCHS)" $@ )
.PHONY: install-exec

install-includes:
	( cd fts ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" include_install_dir=$(include_install_dir) $@ )
.PHONY: install-includes


#
# idb
# creates the SGI idb file and the distribution
#
sgi-pkg:
	/bin/rm -rf /tmp/idb-doc /tmp/idb-exec /tmp/idb-includes
	( RAWIDB=/tmp/idb-doc ; export RAWIDB ; $(MAKE) ARCH=sgi INSTALL="install -idb jmax.doc.documentation" install-doc )
	( RAWIDB=/tmp/idb-exec ; export RAWIDB ; $(MAKE) ARCH=sgi INSTALL="install -idb jmax.sw.exec" install-exec )
	( RAWIDB=/tmp/idb-includes ; export RAWIDB ; $(MAKE) ARCH=sgi INSTALL="install -idb jmax.sw.includes" install-includes )
	/bin/rm -rf ./pkg/sgi/jmax.idb
	cat /tmp/idb-doc /tmp/idb-exec /tmp/idb-includes | sort +4u -6 > ./pkg/sgi/jmax.idb
	/usr/sbin/gendist -verbose -root / -sbase .. -idb ./pkg/sgi/jmax.idb -spec ./pkg/sgi/jmax.spec -dist /usr/dist -all


#
# rpm
# creates a binary rpm
#
bin-rpm:
	rpm -bb --rcfile pkg/rpm/rpmrc pkg/rpm/jmax-local.spec
.PHONY: bin-rpm


#
# new-snapshot, new-patch, new-minor, new-major
# version number manipulation
# 'make new-snapshot' and 'make new-patch' are the most frequent
#
new-snapshot:
	echo "MAJOR=$(MAJOR)" > VERSION
	echo "MINOR=$(MINOR)" >> VERSION
	echo "PATCH_LEVEL=$(PATCH_LEVEL)" >> VERSION
	echo "SNAPSHOT="`echo $(SNAPSHOT) | tr a-yz b-za` >> VERSION

new-patch:
	echo "MAJOR=$(MAJOR)" > VERSION
	echo "MINOR=$(MINOR)" >> VERSION
	echo "PATCH_LEVEL="`expr $(PATCH_LEVEL) + 1` >> VERSION
	echo "SNAPSHOT=a" >> VERSION

new-minor:
	echo "MAJOR=$(MAJOR)" > VERSION
	echo "MINOR="`expr $(MINOR) + 1` >> VERSION
	echo "PATCH_LEVEL=0" >> VERSION
	echo "SNAPSHOT=a" >> VERSION

new-major:
	echo "MAJOR="`expr $(MAJOR) + 1` > VERSION
	echo "MINOR=0" >> VERSION
	echo "PATCH_LEVEL=0" >> VERSION
	echo "SNAPSHOT=a" >> VERSION
