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

ifeq ($(ARCH),sgi)

install-exec:
	$(INSTALL_DIR) $(lib_install_dir)/fts/bin/irix65r5k/opt
	$(INSTALL_DIR) $(lib_install_dir)/fts/bin/irix65r10k/opt
	( cd irix65r5k/opt ; $(INSTALL_SETUID) fts $(lib_install_dir)/fts/bin/irix65r5k/opt )
	( cd irix65r10k/opt ; $(INSTALL_SETUID) fts $(lib_install_dir)/fts/bin/irix65r10k/opt )
	( cd irix65r10k/opt ; $(INSTALL_SETUID) fts.on1 $(lib_install_dir)/fts/bin/irix65r10k/opt )
.PHONY: install-exec

else
ifeq ($(ARCH),linuxpc)

install-exec:
	$(INSTALL_DIR) $(lib_install_dir)fts/bin/linuxpc/opt
	( cd linuxpc/opt ; $(INSTALL_SETUID) fts $(lib_install_dir)fts/bin/linuxpc/opt )
.PHONY: install-exec

endif
endif


# remove the files copied by make install ?
uninstall:
.PHONY: uninstall

