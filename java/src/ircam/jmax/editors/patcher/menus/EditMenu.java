 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
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
/*
    DataFlavor[] flavors = null;
    Transferable clipboardContent = JMaxApplication.getSystemClipboard().getContents(this);
    if( clipboardContent != null)
      flavors = clipboardContent.getTransferDataFlavors();

    if (sketch.isLocked())
      {
	lockItem.setText("Unlock");
	selectAllItem.setEnabled(false);
	cutItem.setEnabled(false);
	copyItem.setEnabled(false);
	duplicateItem.setEnabled(false);
	
	if( clipboardContent != null)
	  pasteItem.setEnabled((flavors != null) && 
			       clipboardContent.isDataFlavorSupported(ErmesSelection.patcherSelectionFlavor));
	else
	  pasteItem.setEnabled( false);

	inspectItem.setEnabled(false);
      }
    else
      {
	lockItem.setText("Lock");

	if (sketch.isTextEditingObject())
	  {
	    // Text editing, look at text selection

	    if (sketch.getSelectedText() != null)
	      {
			 cutItem.setEnabled(true);
			 copyItem.setEnabled(true);
	      }
	    else
	      {
		cutItem.setEnabled(false);
		copyItem.setEnabled(false);
	      }

	    duplicateItem.setEnabled(false);

	    if( clipboardContent != null)
	      pasteItem.setEnabled((flavors != null) &&
				   clipboardContent.isDataFlavorSupported(DataFlavor.stringFlavor));
	    else
	      pasteItem.setEnabled( false);
	  }
	else
	  {
	    if (ErmesSelection.patcherSelection.isEmpty())
	      {
		// Empty selection

		cutItem.setEnabled(false);
		copyItem.setEnabled(false);
		duplicateItem.setEnabled(false);
		inspectItem.setEnabled(false);
		alignMenu.setEnabled(false);
		toFrontMenuItem.setEnabled(false);
		toBackMenuItem.setEnabled(false);
	      }
	    else if (ErmesSelection.patcherSelection.getOwner() == sketch)
	      {
		// Object selection

		cutItem.setEnabled(true);
		copyItem.setEnabled(true);
		duplicateItem.setEnabled(true);
		inspectItem.setEnabled(true);
		alignMenu.setEnabled(true);
		toFrontMenuItem.setEnabled(true);
		toBackMenuItem.setEnabled(true);
	      }

	    if( clipboardContent != null)
	      pasteItem.setEnabled((flavors != null) &&
				   clipboardContent.isDataFlavorSupported(ErmesSelection.patcherSelectionFlavor));
	    else
	      pasteItem.setEnabled( false);
	  }

	selectAllItem.setEnabled(true);	
      }

	 undoAction.putValue(undoAction.NAME, "Undo " + sketch.getUndoType());
	 redoAction.putValue(redoAction.NAME, "Redo " + sketch.getUndoType());
*/
  }
}







