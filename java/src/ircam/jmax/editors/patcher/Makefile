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



ROOTDIR = ../../../../../..
PACKAGE_DIR = ircam/jmax/editors/patcher

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac -deprecation $(JFLAGS) $(SOURCES))
.PHONY: all

DisplayList:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) DisplayList.java)
.PHONY: DisplayList

DisplayObject:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) DisplayObject.java)
.PHONY: DisplayObject

ErmesConnection:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesConnection.java)
.PHONY: ErmesConnection

ErmesDataEditor:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesDataEditor.java)
.PHONY: ErmesDataEditor

ErmesEditor:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesEditor.java)
.PHONY: ErmesEditor

ErmesDrawable:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesDrawable.java)
.PHONY: ErmesDrawable

ErmesModule:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesModule.java)
.PHONY: ErmesModule

EditField:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) EditField.java)
.PHONY: EditField

ErmesObjInOutPop:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesObjInOutPop.java)
.PHONY: ErmesObjInOutPop

ErmesPatcherFactory:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesPatcherFactory.java)
.PHONY: ErmesPatcherFactory

ErmesPatcherListener:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesPatcherListener.java)
.PHONY: ErmesPatcherListener

ErmesSelection:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesSelection.java)
.PHONY: ErmesSelection

ErmesSketchPad:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesSketchPad.java)
.PHONY: ErmesSketchPad

ErmesSketchWindow:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesSketchWindow.java)
.PHONY: ErmesSketchWindow

ErmesTollButton:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesToolButton.java)
.PHONY: ErmesToolButton

ErmesToolbar:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ErmesToolbar.java)
.PHONY: ErmesToolbar

FindPanel:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) FindPanel.java)
.PHONY: FindPanel

FontCache:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) FontCache.java)
.PHONY: FontCache

FpePanel:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) FpePanel.java)
.PHONY: FpePanel

Interaction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Interaction.java)
.PHONY: Interaction

KeyEventClient:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) KeyEventClient.java)
.PHONY: KeyEventClient

MultiLineText:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MultiLineText.java)
.PHONY: MultiLineText

Settings:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) Settings.java)
.PHONY: Settings

ObjectAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ObjectAction.java)
.PHONY: ObjectAction


include $(ROOTDIR)/java/Makefiles/Makefile.misc
