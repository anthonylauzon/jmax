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

FILES_TO_INSTALL=\
config_io.gif		\
config_sys.gif		\
doc_compile.gif		\
doc_ctrl_obj.gif	\
doc_dev.gif		\
doc_dsp_obj.gif		\
doc_kernel_ref.gif	\
doc_release.gif		\
doc_user.gif		\
getting.gif		\
installation.gif	\
ircam_banner.gif	\
jmax_logo_tiny.gif	\
language.gif		\
packages.gif		\
patcher.gif		\
projects.gif		\
variable.gif		\

install:
	$(INSTALL_DIR) $(doc_install_dir)/images
	for f in $(FILES_TO_INSTALL) ; do $(INSTALL_DATA) $$f $(doc_install_dir)/images ; done
.PHONY: install

# remove the files copied by make install ?
uninstall:
.PHONY: uninstall

