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

include VERSION
JMAX_VERSION_TAG=V$(JMAX_MAJOR_VERSION)_$(JMAX_MINOR_VERSION)_$(JMAX_PATCH_VERSION)$(JMAX_VERSION_STATUS)

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

# This is for Ctags 4.0.2
NTAGS:
	find . \! \( -name '*~' \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.scm" -o -name "*.tcl" -name "*.jpk" \) -print | etags -L -
.PHONY: NTAGS

#
# cvs-tag
#
cvs-tag: spec-files
	if cvs -n update 2>&1 | egrep '^[ACMPRU] ' ; then \
		echo "Not sync with cvs (do an update or commit)" ; \
		exit 1 ; \
	fi
	( \
		echo "Tagging with tag $(JMAX_VERSION_TAG)" ; \
		cvs tag -F $(JMAX_VERSION_TAG) ; \
	)
.PHONY: cvs-tag

#
# spec_files
# update the spec files for version number
#
spec-files:
	$(MAKE) -C utils/rpm $@
#	$(MAKE) -C utils/sgi $@
.PHONY: spec-files

#
# dist
# does a cvs export and a .tar.gz of the sources
#
dist: cvs-tag
	( \
		umask 22 ; \
		mkdir .$$$$ ; \
		cd .$$$$ ; \
		cvs export -r$(JMAX_VERSION_TAG) jmax ; \
		mv jmax jmax-$(JMAX_VERSION) ; \
		tar cvf - jmax-$(JMAX_VERSION) | gzip -c --best > ../../jmax-$(VERSION)-src.tar.gz ; \
		chmod 644 ../../jmax-$(JMAX_VERSION)-src.tar.gz ; \
		cd .. ; \
		/bin/rm -rf .$$$$ ; \
	)
.PHONY: dist

#
# release
# makes a final release
#
release: cvs-tag dist


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
	$(MAKE) -C tutorials install 
# The following line is a hack that installs the <package>.jpk file on all platforms,
# so that you can load the sgidev package even if you are running the GUI on Linux.
	$(MAKE) -C packages install-noarch 
	$(MAKE) -C tuorials install-noarch 
.PHONY: install-bin

install-includes:
	$(MAKE) -C include install
.PHONY: install-includes


#
# new-patch, new-minor, new-major
# version number manipulation (new-patch is the most frequent, new-major the least...)
#
SNAPSHOT_STATUS=-latest-cvs

new-patch:
	awk -F= -v x="PATCH" -v y="$(SNAPSHOT_STATUS)" '\
$$1 ~ x { printf( "%s=%d\n", $$1, $$2+1); }\
$$1 ~ "STATUS" { printf( "%s=%s\n", $$1, y); }\
$$1 !~ x && $$1 !~ "STATUS" { print; }' VERSION > VERSION.out
	mv VERSION.out VERSION
	$(MAKE) spec-files

new-minor:
	awk -F= -v x="MINOR" -v y="$(SNAPSHOT_STATUS)" '\
$$1 ~ x { printf( "%s=%d\n", $$1, $$2+1); }\
$$1 ~ "STATUS" { printf( "%s=%s\n", $$1, y); }\
$$1 !~ x && $$1 !~ "STATUS" { print; }' VERSION > VERSION.out
	mv VERSION.out VERSION
	$(MAKE) spec-files

new-major:
	awk -F= -v x="MAJOR" -v y="$(SNAPSHOT_STATUS)" '\
$$1 ~ x { printf( "%s=%d\n", $$1, $$2+1); }\
$$1 ~ "STATUS" { printf( "%s=%s\n", $$1, y); }\
$$1 !~ x && $$1 !~ "STATUS" { print; }' VERSION > VERSION.out
	mv VERSION.out VERSION
	$(MAKE) spec-files

final:
	awk -F= '\
$$1 ~ "STATUS" { printf( "%s=\n", $$1); }\
$$1 !~ "STATUS" { print; }' VERSION > VERSION.out
	mv VERSION.out VERSION
	$(MAKE) spec-files


