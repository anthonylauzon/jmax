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

all_c: all
clean_c: clean

all:
	(cd sys; $(MAKE) all)
	(cd lang; $(MAKE) all)
	(cd runtime; $(MAKE) all)
	(cd tiles; $(MAKE) all)
.PHONY: all

clean:
	(cd sys; $(MAKE) clean)
	(cd lang; $(MAKE) clean)
	(cd runtime; $(MAKE) clean)
	(cd tiles; $(MAKE) clean)
.PHONY: all

install:
	$(INSTALL_DIR) $(include_install_dir)fts
	for f in *.h ; do $(INSTALL_DATA) $$f $(include_install_dir)fts ; done
	for d in lang runtime sys ; do (cd $$d ; $(MAKE) install ) ; done
.PHONY: install

# remove the files copied by make install ?
uninstall:
.PHONY: uninstall
