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

JMAXDISTDIR=../..
include $(JMAXDISTDIR)/Makefiles/Makefile.$(ARCH)

all:
	(cd java ; $(MAKE) all)	
	(cd src; $(MAKE) -k all)
.PHONY: all

clean:
	(cd java ; $(MAKE) clean)	
	(cd src; $(MAKE) clean)
.PHONY: clean

all_java:
	(cd java ; $(MAKE) all)
.PHONY: all

clean_java:
	(cd java ; $(MAKE) clean)
.PHONY: clean

install:
	$(INSTALL_DIR) $(lib_install_dir)/packages/ispw
	$(INSTALL_DATA) ispw.jpk $(lib_install_dir)/packages/ispw/ispw.jpk
	$(INSTALL_DATA) ispw.scm $(lib_install_dir)/packages/ispw/ispw.scm
	( cd images; $(MAKE) $@)
	( cd abs ; $(MAKE) $@)
	( cd help; $(MAKE) $@)
	( cd sounds; $(MAKE) $@)
	( cd templates; $(MAKE) $@)
	( cd java; $(MAKE) $@)
	$(INSTALL_DIR) $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt
	( \
		cd lib/$(ARCH)/opt ; \
		$(INSTALL_LIB) libcontrol.so $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt/libcontrol.so ; \
		$(INSTALL_LIB) libdata.so $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt/libdata.so ; \
		$(INSTALL_LIB) libfilters.so $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt/libfilters.so ; \
		$(INSTALL_LIB) libio.so $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt/libio.so ; \
		$(INSTALL_LIB) libmidi.so $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt/libmidi.so ; \
		$(INSTALL_LIB) libpitch.so $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt/libpitch.so ; \
		$(INSTALL_LIB) libsampling.so $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt/libsampling.so ; \
		$(INSTALL_LIB) libsignal.so $(lib_install_dir)/packages/ispw/lib/$(ARCH)/opt/libsignal.so ; \
	)
.PHONY: install
