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
	(cd control; $(MAKE)  all)
	(cd data; $(MAKE)  all)
	(cd filters; $(MAKE)  all)
	(cd io; $(MAKE)  all)
	(cd midi; $(MAKE)  all)
	(cd sampling; $(MAKE)  all)
	(cd pitch; $(MAKE)  all)
	(cd signal; $(MAKE)  all)
.PHONY: all

clean:
	(cd control; $(MAKE) clean)
	(cd data; $(MAKE) clean)
	(cd filters; $(MAKE) clean)
	(cd io; $(MAKE) clean)
	(cd midi; $(MAKE) clean)
	(cd sampling; $(MAKE) clean)
	(cd pitch; $(MAKE) clean)
	(cd signal; $(MAKE) clean)
.PHONY: clean
