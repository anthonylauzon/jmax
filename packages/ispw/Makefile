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

all:
	(cd src; $(MAKE) -k all)
.PHONY: all

clean:
	(cd src; $(MAKE) clean)
.PHONY: clean

install:
	$(INSTALL_DIR) $(lib_install_dir)/packages/ispw
	$(INSTALL_DATA) ispw.jpk $(lib_install_dir)/packages/ispw
	for d in $(SUB_ARCHS) ; do $(INSTALL_DIR) $(lib_install_dir)packages/ispw/lib/$$d/opt ; done
	for d in $(SUB_ARCHS) ; do \
		( cd lib/$$d/opt ; \
			$(INSTALL_DATA) libcontrol.so $(lib_install_dir)packages/ispw/lib/$$d/opt ; \
			$(INSTALL_DATA) libdata.so $(lib_install_dir)packages/ispw/lib/$$d/opt ; \
			$(INSTALL_DATA) libfilters.so $(lib_install_dir)packages/ispw/lib/$$d/opt ; \
			$(INSTALL_DATA) libio.so $(lib_install_dir)packages/ispw/lib/$$d/opt ; \
			$(INSTALL_DATA) libmidi.so $(lib_install_dir)packages/ispw/lib/$$d/opt ; \
			$(INSTALL_DATA) libpitch.so $(lib_install_dir)packages/ispw/lib/$$d/opt ; \
			$(INSTALL_DATA) libsampling.so $(lib_install_dir)packages/ispw/lib/$$d/opt ; \
			$(INSTALL_DATA) libsignal.so $(lib_install_dir)packages/ispw/lib/$$d/opt ; \
		) ; done
	( cd abs ; $(MAKE) install)
	( cd help; $(MAKE) install)
	( cd sounds; $(MAKE) install)
	( cd templates; $(MAKE) install)
.PHONY: install

uninstall:
.PHONY: uninstall
