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


ROOTDIR = ../../../../../../..
PACKAGE_DIR = ircam/jmax/editors/patcher/actions

include $(ROOTDIR)/java/Makefiles/Makefile.common

INCLUDE_PATH := $(JAVA_SRC_ROOT)/$(PACKAGE_DIR)
include Sources2

all:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) $(SOURCES))
.PHONY: all

PatcherAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) PatcherAction.java)
.PHONY: PatcherAction

OpenAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) OpenAction.java)
.PHONY: OpenAction

NewAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) NewAction.java)
.PHONY: NewAction

CloseAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) CloseAction.java
.PHONY: CloseAction

PrintAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) PrintAction.java)
.PHONY: PrintAction

QuitAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) QuitAction.java)
.PHONY: QuitAction

SaveAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) SaveAction.java)
.PHONY: SaveAction

SaveAsAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) SaveAsAction.java)
.PHONY: SaveAsAction

SaveToAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) SaveToAction.java)
.PHONY: SaveToAction

StatisticsAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) StatisticsAction.java)
.PHONY: StatisticsAction


CutAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) CutAction.java)
.PHONY: CutAction

CopyAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) CopyAction.java)
.PHONY: CopyAction

PasteAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) PasteAction.java)
.PHONY: PasteAction

DuplicateAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) DuplicateAction.java)
.PHONY: DuplicateAction

SelectAllAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) SelectAllAction.java)
.PHONY: SelectAllAction

FindAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) FindAction.java)
.PHONY: FindAction

FindErrorsAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) FindErrorsAction.java)
.PHONY: FindErrorsAction

InspectAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) InspectAction.java)
.PHONY: InspectAction

LockAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) LockAction.java)
.PHONY: LockAction

AlignBottomAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) AlignBottomAction.java)
.PHONY: AlignBottomAction

AlignLeftAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) AlignLeftAction.java)
.PHONY: AlignLeftAction

AlignRightAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) AlignRightAction.java)
.PHONY: AlignRightAction

AlignTopAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) AlignTopAction.java)
.PHONY: AlignTopAction


ResizeAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ResizeAction.java)
.PHONY: ResizeAction

MoveAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) MoveAction.java)
.PHONY: MoveAction

ShowErrorAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ShowErrorAction.java)
.PHONY: ShowErrorAction

ResizeToMaxWidthAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ResizeToMaxWidthAction.java)
.PHONY: ResizeToMaxWidthAction

ResizeToMaxHeightAction:
	(CLASSPATH=$(JMAX_CLASSPATH) ; export CLASSPATH ; javac $(JFLAGS) ResizeToMaxHeightAction.java)
.PHONY: ResizeToMaxHeightAction




include $(ROOTDIR)/java/Makefiles/Makefile.misc
