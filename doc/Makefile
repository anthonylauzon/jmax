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
# Authors: Francois Dechelle, Norbert Schnell.
#

JMAXDISTDIR=..
include $(JMAXDISTDIR)/Makefiles/Makefile.$(ARCH)

all:
#	(cd api ; $(MAKE) $@)

install:
	for f in *.html ; do $(INSTALL_DATA) $$f $(doc_install_dir) ; done
#	(cd api ; $(MAKE) $@)
	(cd images ; $(MAKE) $@)
.PHONY: install

webdoc:
	for f in *.html ; do $(INSTALL_DATA) $$f $(webdoc_dir) ; done
	$(INSTALL_DIR) $(webdoc_dir)/images
	$(INSTALL_DIR) $(webdoc_dir)/api/html
	( \
		cd images ; \
		for f in *.gif ; do $(INSTALL_DATA) $$f $(webdoc_dir)/images/$$f ; done \
	)
	( \
		cd ../images ; \
		for f in *.gif ; do $(INSTALL_DATA) $$f $(webdoc_dir)/images/$$f ; done \
	)
	( \
		cd ../packages/guiobj/images/ ; \
		for f in *.gif ; do $(INSTALL_DATA) $$f $(webdoc_dir)/images/$$f ; done \
	)
#	( \
#		cd api/html ; \
#		$(INSTALL_DATA) doxygen.css $(webdoc_dir)/api/html/doxygen.css ; \
#		$(INSTALL_DATA) doxygen.gif $(webdoc_dir)/api/html/doxygen.gif ; \
#		for f in *.html ; do $(INSTALL_DATA) $$f $(webdoc_dir)/api/html/$$f ; done ; \
#	)
.PHONY: webdoc
