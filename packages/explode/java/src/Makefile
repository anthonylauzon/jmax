# 
# jMax
# Copyright (C) 1999 by IRCAM
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
# Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#

ROOTDIR = ../../../..
PACKAGE_DIR =  $(ROOTDIR)/packages/explode


include $(ROOTDIR)/packages/explode/java/Makefiles/Makefile.common

ALL_PACKAGES =  ircam.jmax.editors.explode

all: 
	(cd ircam ; $(MAKE) all)
.PHONY: all

doc:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javadoc -d $(PACKAGE_DIR)/java/doc $(ALL_PACKAGES))
.PHONY: doc

doc_private:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javadoc -private -d $(ROOTDIR)/java/doc_private $(ALL_PACKAGES))
.PHONY: doc_private

jar:  
	(cd $(ROOTDIR)/packages/explode/java/classes ; /bin/rm -f explode.jar ; jar -cvf explode.jar ircam )
.PHONY: jar


include $(ROOTDIR)/packages/explode/java/Makefiles/Makefile.misc
