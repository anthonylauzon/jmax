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

JMAXDISTDIR=../../..
include $(JMAXDISTDIR)/Makefiles/Makefile.$(ARCH)

lib-recursive:
	(cd mess; $(MAKE) lib)
	(cd utils; $(MAKE) lib)
	(cd datalib; $(MAKE) lib)
	(cd ftl; $(MAKE) lib)
	(cd dsp; $(MAKE) lib)
	(cd veclib; $(MAKE) lib-recursive)
.PHONY: lib-recursive

cleanobjs-recursive:
	(cd mess; $(MAKE) cleanobjs)
	(cd utils; $(MAKE) cleanobjs)
	(cd datalib; $(MAKE) cleanobjs)
	(cd ftl; $(MAKE) cleanobjs)
	(cd dsp; $(MAKE) cleanobjs)
	(cd veclib; $(MAKE) cleanobjs-recursive)
.PHONY: cleanobjs-recursive

install:
	$(INSTALL_DIR) $(include_install_dir)/fts/lang
	for f in *.h ; do $(INSTALL_DATA) $$f $(include_install_dir)/fts/lang ; done
	for d in datalib dsp ftl mess utils veclib; do ( cd $$d ; $(MAKE) $@ ; ) ; done
.PHONY: install

