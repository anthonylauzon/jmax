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



ROOTDIR = ../../..

include $(ROOTDIR)/Makefiles/Makefile.common
include Sources

all: $(LIBDIR) $(OBJDIR) $(LIBDIR)/libDevices.a

$(LIBDIR)/libDevices.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libDevices.a $(OBJECTS)

install-includes:
	$(INSTALL_DIR) $(include_install_dir)fts/runtime/devices
	for f in *.h ; do $(INSTALL_DATA) $$f $(include_install_dir)fts/runtime/devices ; done
.PHONY: install-includes

# remove the files copied by make install ?
uninstall:
.PHONY: uninstall

TOCLEAN = $(LIBDIR)/libDevices.a

include $(ROOTDIR)/Makefiles/Makefile.misc





