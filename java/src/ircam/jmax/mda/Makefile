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



ROOTDIR = ../../../../..
PACKAGE_DIR = ircam/jmax/mda

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all: 
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

MaxData:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxData.java)
.PHONY: MaxData

MaxDataEditor:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataEditor.java)
.PHONY: MaxDataEditor

MaxDataEditorFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDataEditorFactory.java)
.PHONY: MaxDataEditorFactory

MaxDocument:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDocument.java)
.PHONY: MaxDocument

MaxDocumentException:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDocumentException.java)
.PHONY: MaxDocumentException

MaxDocumentHandler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDocumentHandler.java)
.PHONY: MaxDocumentHandler

MaxDocumentType:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxDocumentType.java)
.PHONY: MaxDocumentType

MaxTclDocument:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclDocument.java)
.PHONY: MaxTclDocument

MaxTclFileDocumentHandler:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclFileDocumentHandler.java)
.PHONY: MaxTclFileDocumentHandler

MaxTclInterpreter:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxTclInterpreter.java)
.PHONY: MaxTclInterpreter

Mda:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Mda.java)
.PHONY: Mda

MdaModule:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MdaModule.ja)
.PHONY: MdaModule

MaxEnv:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MaxEnv.java)
.PHONY: MaxEnv

include $(ROOTDIR)/java/Makefiles/Makefile.misc




