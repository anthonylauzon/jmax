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
	(cd fts; $(MAKE) all)
	(cd java ; $(MAKE) all)
	(cd lib; $(MAKE) all)
	(cd packages; $(MAKE) all)

all_c:
	(cd fts; $(MAKE) all)
	(cd lib; $(MAKE) all_c)
	(cd packages; $(MAKE) all_c)

all_java:
	(cd java ; $(MAKE) all)
	(cd lib; $(MAKE) all_java)
	(cd packages; $(MAKE) all_java)

clean:
	(cd fts; $(MAKE) clean)
	(cd java ; $(MAKE) clean)
	(cd lib; $(MAKE) clean)
	(cd packages; $(MAKE) clean)

clean_c:
	(cd fts; $(MAKE) clean)
	(cd packages; $(MAKE) clean_c)
	(cd lib; $(MAKE) clean_c)

clean_java:
	(cd java ; $(MAKE) clean)
	(cd lib; $(MAKE) clean_java)
	(cd packages; $(MAKE) clean_java)

tags:
	(find . \! \( -name '*~' \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.tcl" \) -print | etags -t - )
.PHONY: tags

.PHONY: all
.PHONY: all_c
.PHONY: all_java

.PHONY: clean
.PHONY: clean_c
.PHONY: clean_java

