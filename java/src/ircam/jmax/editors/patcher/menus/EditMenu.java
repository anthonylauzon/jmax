 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

// import javax.swing.*;
import javax.swing.AbstractButton;
import javax.swing.JMenuItem;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.toolkit.actions.*;

/** Implement the editor File Menu */

public class EditMenu extends EditorMenu
{
  ErmesSketchPad sketch;

  public EditorAction cutAction       			= new CutAction();
  public EditorAction copyAction      			= new CopyAction();
  public EditorAction pasteAction     			= new PasteAction();
  public EditorAction duplicateAction 			= new DuplicateAction();
  public EditorAction selectAllAction 			= new SelectAllAction();
  public EditorAction findAction      			= new FindAction();
  public EditorAction inspectAction   			= new InspectAction();
  public EditorAction lockAction      			= new LockAction();
  public EditorAction alignTopAction    		= new AlignTopAction();
  public EditorAction alignLeftAction   		= new AlignLeftAction();
  public EditorAction alignBottomAction 		= new AlignBottomAction();
  public EditorAction alignRightAction  		= new AlignRightAction();
  public EditorAction undoAction				= new UndoAction();
  public EditorAction redoAction				= new RedoAction(); 
  public EditorAction bringToFrontAction 		= new BringToFrontAction();
  public EditorAction sendToBackAction  		= new SendToBackAction();

  public EditMenu(ErmesSketchPad sketch)
  {
    super("Edit");
	setMnemonic(KeyEvent.VK_E);

    this.sketch = sketch;

    setHorizontalTextPosition(AbstractButton.LEFT);

    add(cutAction);
    add(copyAction);
    add(pasteAction);
    add(duplicateAction);
    addSeparator();
    add(undoAction);
    add(redoAction);
    addSeparator();
    add(inspectAction);
    addSeparator();
    add(Actions.scalePatcherAction);
    addSeparator();
    add(selectAllAction);
    addSeparator();
    add(findAction);
    addSeparator();
    AlignMenu alignMenu = new AlignMenu(this);
    add(alignMenu);
    addSeparator();
    add(bringToFrontAction);
    add(sendToBackAction);
    addSeparator();
    add(lockAction);
  }

  public void updateMenu()
  {
		if(sketch.isLocked())
			lockAction.putValue(EditorAction.NAME, "Unlock");
		else
			lockAction.putValue(EditorAction.NAME, "Lock");
  }
}







