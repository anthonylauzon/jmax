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

JMAXROOTDIR= ..
DISTDIR=$(JMAXROOTDIR)

ifdef ARCH
include $(JMAXROOTDIR)/Makefiles/Makefile.$(ARCH)
endif

# Architecture dependant packages are supposed to be C...
C_PACKAGES=$(ARCH_PACKAGES) reftype system io guiobj mess control data numeric math ispw ispwmath lists sources
JAVA_PACKAGES=table
MIXED_PACKAGES=explode qlist

all: all_c all_java
.PHONY: all

all_c:
	for i in $(C_PACKAGES) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) -k all ; fi) ; done
	for i in $(MIXED_PACKAGES) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) -k all_c ; fi ) ; done
.PHONY: all_c

all_java:
	for i in $(JAVA_PACKAGES) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) -k all ; fi ) ; done
	for i in $(MIXED_PACKAGES) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) -k all_java ; fi ) ; done
.PHONY: all_java

clean: clean_c clean_java
.PHONY: clean

clean_c:
	for i in $(C_PACKAGES) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) clean ; fi ) ; done
	for i in $(MIXED_PACKAGES) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) clean_c ; fi ) ; done
.PHONY: clean_c

clean_java:
	for i in $(JAVA_PACKAGES) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) clean ; fi ) ; done
	for i in $(MIXED_PACKAGES) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) clean_java ; fi ) ; done
.PHONY: clean_java

install:
	$(INSTALL_DIR) $(lib_install_dir)/packages
	for i in $(C_PACKAGES) ; do ( cd $$i; $(MAKE) $@) ; done
	for i in $(JAVA_PACKAGES) ; do ( cd $$i; $(MAKE) $@ ) ; done
	for i in $(MIXED_PACKAGES) ; do ( cd $$i; $(MAKE) $@ ) ; done
.PHONY: install


