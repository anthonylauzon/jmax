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

JMAXROOTDIR = ../../../..
ROOTDIR = ../../../..
PACKAGE_DIR = 

MAX_PACKAGE_NAME = qlist

include $(JMAXROOTDIR)/java/Makefiles/Makefile.package

DOC_PACKAGES =  ircam.jmax.editors.qlist

all: $(JAVA_CLASSES_ROOT)
	(cd ircam ; $(MAKE) MAX_PACKAGE_NAME=$(MAX_PACKAGE_NAME) all)
.PHONY: all

clean:
	(cd ircam ; $(MAKE) MAX_PACKAGE_NAME=$(MAX_PACKAGE_NAME) clean)
.PHONY: clean

doc:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javadoc -d $(PACKAGE_DIR)/java/doc $(DOC_PACKAGES))
.PHONY: doc

doc_private:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javadoc -private -d $(ROOTDIR)/java/doc_private $(ALL_PACKAGES))
.PHONY: doc_private

jar:  
	(cd $(ROOTDIR)/packages/qlist/java/classes ; /bin/rm -f qlist.jar ; jar -cvf qlist.jar ircam	)
.PHONY: jar


include $(JMAXROOTDIR)/java/Makefiles/Makefile.package.misc
