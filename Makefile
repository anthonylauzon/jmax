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

ifdef ARCH
include Makefiles/Makefile.$(ARCH)
endif

include VERSION

version=$(MAJOR).$(MINOR).$(PATCH_LEVEL)$(SNAPSHOT)
disttag=V$(MAJOR)_$(MINOR)_$(PATCH_LEVEL)$(SNAPSHOT)
distdir=jmax-$(version)

all:
	(cd fts; $(MAKE) all)
	(cd java ; $(MAKE) all)
	(cd lib; $(MAKE) all)
	(cd packages; $(MAKE) all)
	(cd syspackages; $(MAKE) all)
.PHONY: all

all_c:
	(cd fts; $(MAKE) all)
	(cd lib; $(MAKE) all_c)
	(cd packages; $(MAKE) all_c)
	(cd syspackages; $(MAKE) all)
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
	(cd syspackages; $(MAKE) clean)
.PHONY: clean

clean_c:
	(cd fts; $(MAKE) clean)
	(cd packages; $(MAKE) clean_c)
	(cd lib; $(MAKE) clean_c)
	(cd syspackages; $(MAKE) clean)
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
# sgi
# target for SGI Irix 6.5 processors R5000 and R10000
#
sgi:
	$(MAKE) ARCH=r5k-irix6.5 all
	$(MAKE) ARCH=r10k-irix6.5 all
.PHONY: sgi

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
	( cd bin ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DIR="$(INSTALL_DIR)" bin_install_dir=$(bin_install_dir) install-noarch )
	$(INSTALL_DIR) $(lib_install_dir)
	( cd config ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) install-noarch )
	( cd images ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) install-noarch )
	( cd java ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) install-noarch )
	( cd scm ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) install-noarch )
	( cd tcl ; $(MAKE) INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) install-noarch )
	for a in $(INSTALL_ARCHS) ; do \
		( cd fts ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_SETUID="$(INSTALL_SETUID)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) ARCH=$$a install-arch ) ; \
	done
	( cd packages ; $(MAKE) INSTALL_LIB="$(INSTALL_LIB)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) install-noarch )
	for a in $(INSTALL_ARCHS) ; do \
		( cd packages ; $(MAKE) INSTALL_LIB="$(INSTALL_LIB)" INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) ARCH=$$a install-arch ) ; \
	done
	( cd syspackages ; $(MAKE) INSTALL_LIB="$(INSTALL_LIB)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) install-noarch )
	for a in $(INSTALL_ARCHS) ; do \
		( cd syspackages ; $(MAKE) INSTALL_LIB="$(INSTALL_LIB)" INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" lib_install_dir=$(lib_install_dir) ARCH=$$a install-arch ) ; \
	done
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
	( MAXHOME=`/bin/pwd` ; export MAXHOME ; $(MAKE) -s ARCH=sgi INSTALL="$${MAXHOME}/bin/mksgipkg jmax.doc.documentation " prefix=usr install-doc > /tmp/rawidb )
	( MAXHOME=`/bin/pwd` ; export MAXHOME ; $(MAKE) -s ARCH=sgi INSTALL="$${MAXHOME}/bin/mksgipkg jmax.sw.exec " prefix=usr install-exec >> /tmp/rawidb )
	( MAXHOME=`/bin/pwd` ; export MAXHOME ; $(MAKE) -s ARCH=sgi INSTALL="$${MAXHOME}/bin/mksgipkg jmax.sw.includes " prefix=usr install-includes >> /tmp/rawidb )
	cat /tmp/rawidb | sort +4u -6 > ./pkg/sgi/jmax.idb
	/usr/sbin/gendist -verbose -root / -sbase . -idb ./pkg/sgi/jmax.idb -spec ./pkg/sgi/jmax.spec -dist /usr/dist -all


#
# rpm
# creates a binary rpm
#
bin-rpm:
	rpm -bb --rcfile pkg/rpm/rpmrc pkg/rpm/jmax-local.spec
.PHONY: bin-rpm

# For redhat 6
bin-rpm6:
	( cd pkg/rpm ; rpm -bb --rcfile /usr/lib/rpm/rpmrc:rpmrc6 jmax-local.spec)
.PHONY: bin-rpm6


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
