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

ROOTDIR = ../..
PACKAGE_DIR = 

include $(ROOTDIR)/java/Makefiles/Makefile.common

# this list can be obtained by:
# $ find . -type d \! \( -name CVS -prune \)
# plus removing :
# ./ircam/jmax/editors	
# ./ircam/jmax/editors/project	
PACKAGES = \
ircam/jmax	\
ircam/jmax/fts	\
ircam/jmax/mda	\
ircam/jmax/tcl	\
ircam/jmax/dialogs	\
ircam/jmax/script	\
ircam/jmax/script/pkg	\
ircam/jmax/script/scm	\
ircam/jmax/script/tcl	\
ircam/jmax/widgets	\
ircam/jmax/utils	\
ircam/jmax/editors/patcher	\
ircam/jmax/editors/patcher/interactions	\
ircam/jmax/editors/patcher/actions	\
ircam/jmax/editors/patcher/objects	\
ircam/jmax/editors/patcher/menus	\
ircam/jmax/editors/console	\
ircam/jmax/editors/control	\
ircam/jmax/toolkit	\
ircam/jmax/toolkit/actions	\
ircam/jmax/toolkit/menus	\

JAR_FILE= jmax.jar

ALL_CLASSES=\
	$(shell find $(JAVA_CLASSES_ROOT)/ircam -name '*.class') \
	$(shell find $(JAVA_CLASSES_ROOT)/silk -name '*.class') \

all: $(JAVA_CLASSES_ROOT)
	(cd ircam ; $(MAKE) $@)
	(cd silk ; $(MAKE) $@)
.PHONY: all

clean:
	(cd ircam ; $(MAKE) $@)
	(cd silk ; $(MAKE) $@)
.PHONY: clean

doc:  
	(CLASSPATH=$(JMAX_CLASSPATH); export CLASSPATH ; javadoc -d $(ROOTDIR)/java/doc $(PACKAGES))
.PHONY: doc

doc_private:  
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javadoc -private -d $(ROOTDIR)/java/doc_private $(PACKAGES))
.PHONY: doc_private

jar: $(JAVA_CLASSES_ROOT)/$(JAR_FILE)
.PHONY: jar

$(JAVA_CLASSES_ROOT)/$(JAR_FILE): $(ALL_CLASSES)
	(cd $(JAVA_CLASSES_ROOT) ; /bin/rm -f $(JAR_FILE) ; jar -cvf $(JAR_FILE) ircam silk )

include $(ROOTDIR)/java/Makefiles/Makefile.misc



