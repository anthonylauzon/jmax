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
version=$(MAJOR).$(MINOR).$(PATCH_LEVEL).$(SNAPSHOT)
disttag=substV_$(MAJOR)_$(MINOR)_$(PATCH_LEVEL).$(SNAPSHOT)
else
version=$(MAJOR).$(MINOR).$(PATCH_LEVEL)
disttag=V_$(MAJOR)_$(MINOR)_$(PATCH_LEVEL)
endif
distdir=jmax-$(version)


ifeq ($(ARCH),sgi)
INSTALL=install
INSTALL_DATA=$(INSTALL) -m 644
INSTALL_PROGRAM=$(INSTALL) -m 755
INSTALL_DIR=$(INSTALL) -d -m 755
prefix=/usr
doc_install_dir=$(prefix)/webdocs/jmax/$(version)/
lib_install_dir=$(prefix)/lib/jmax/$(version)/
include_install_dir=$(prefix)/include/
SUB_ARCHS=irix65r10k irix65r5k
else
ifeq ($(ARCH),linuxpc)
INSTALL=install
INSTALL_DATA=$(INSTALL) --mode=0644
INSTALL_PROGRAM=$(INSTALL) --mode=0755
INSTALL_DIR=$(INSTALL) -d --mode=0755
prefix=/usr
doc_install_dir=$(prefix)/doc/jmax/$(version)/
lib_install_dir=$(prefix)/lib/jmax/$(version)/
include_install_dir=$(prefix)/include/
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

TAGS:
	find . \! \( -name '*~' \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.tcl" -o -name "*.html" \) -print | etags -t -
.PHONY: TAGS

cvstag:
	cvs tag -F $(disttag)
.PHONY: cvstag

dist: cvstag
	rm -rf $(distdir)
	mkdir $(distdir)
	cvs export -r $(disttag) -d $(distdir) max
	tar cvf - $(distdir) | gzip -c --best > $(distdir).tar.gz 
	rm -rf $(distdir)
.PHONY: dist

# copies the files to $(prefix)/...
install:
	$(INSTALL_DIR) $(doc_install_dir)
	$(INSTALL_DIR) $(lib_install_dir)
	$(INSTALL_DATA) LICENCE.fr $(doc_install_dir)
	$(INSTALL_DATA) LICENSE $(doc_install_dir)
	$(INSTALL_DATA) README $(doc_install_dir)
	$(INSTALL_DATA) VERSION $(doc_install_dir)
	( cd bin ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" prefix=$(prefix) install )
	( cd doc ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" prefix=$(prefix) doc_install_dir=$(doc_install_dir) install )
	( cd config ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" prefix=$(prefix) lib_install_dir=$(lib_install_dir) install )
	( cd fts ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" prefix=$(prefix) lib_install_dir=$(lib_install_dir) include_install_dir=$(include_install_dir) install )
	( cd images ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" prefix=$(prefix) lib_install_dir=$(lib_install_dir) install )
	( cd java ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" prefix=$(prefix) lib_install_dir=$(lib_install_dir) install )
	( cd tcl ; $(MAKE) install INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" prefix=$(prefix) lib_install_dir=$(lib_install_dir) install )
	( cd packages ; $(MAKE) INSTALL_PROGRAM="$(INSTALL_PROGRAM)" INSTALL_DATA="$(INSTALL_DATA)" INSTALL_DIR="$(INSTALL_DIR)" prefix=$(prefix) lib_install_dir=$(lib_install_dir) SUB_ARCHS="$(SUB_ARCHS)" install )
.PHONY: install

# remove the files copied by make install ?
uninstall:
.PHONY: uninstall

