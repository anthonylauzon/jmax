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
	(cd c ; $(MAKE) all)
.PHONY: all

clean:
	(cd c ; $(MAKE) clean)
.PHONY: clean

all_c:
	(cd c ; $(MAKE) all)
.PHONY: all_c

clean_c:
	(cd c ; $(MAKE) clean)
.PHONY: clean_c

all_java:
.PHONY: all_java

clean_java:
.PHONY: clean_java

install:
	$(INSTALL_DIR) $(lib_install_dir)/packages/midi
	$(INSTALL_DATA) midi.jpk $(lib_install_dir)/packages/midi/midi.jpk
	( cd c ; $(MAKE) $@)
	( cd help ; $(MAKE) $@)
.PHONY: install
