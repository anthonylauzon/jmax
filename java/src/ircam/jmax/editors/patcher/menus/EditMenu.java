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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

/** Implement the editor File Menu */

public class EditMenu extends EditorMenu
{
  ErmesSketchPad sketch;

  JMenuItem cutItem;
  JMenuItem copyItem;
  JMenuItem pasteItem;
  JMenuItem duplicateItem;
  JMenuItem selectAllItem;
  JMenuItem lockItem;
  AlignMenu alignMenu;
  JMenuItem toFrontMenuItem;
  JMenuItem toBackMenuItem;

  public EditMenu(ErmesSketchPad sketch)
  {
    super("Edit");

    this.sketch = sketch;

    setHorizontalTextPosition(AbstractButton.LEFT);

    cutItem       = add(Actions.cutAction, "Cut", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_X);
    copyItem      = add(Actions.copyAction, "Copy", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_C);
    pasteItem     = add(Actions.pasteAction, "Paste", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_V);
    duplicateItem = add(Actions.duplicateAction, "Duplicate", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_D);

    addSeparator();

    JMenuItem scaleItem = add(Actions.scalePatcherAction, "Rescale Window");

    addSeparator();

    selectAllItem = add(Actions.selectAllAction, "SelectAll", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_A);

    addSeparator();

    add(Actions.findAction, "Find", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_F);

    addSeparator();


    alignMenu = new AlignMenu();
    add(alignMenu);

    addSeparator();

    toFrontMenuItem = add(Actions.bringToFrontAction, "Bring To Front", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_U);
    toBackMenuItem  = add(Actions.sendToBackAction, "Send To Back", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_B);

    addSeparator();

    lockItem = add(Actions.lockAction, "Lock", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_E);
  }

  public void updateMenu()
  {
    Transferable clipboardContent = MaxApplication.systemClipboard.getContents(this);
    DataFlavor[] flavors = clipboardContent.getTransferDataFlavors();

    if (sketch.isLocked())
      {
	lockItem.setText("Unlock");
	selectAllItem.setEnabled(false);
	cutItem.setEnabled(false);
	copyItem.setEnabled(false);
	duplicateItem.setEnabled(false);
	
	pasteItem.setEnabled((flavors != null) &&
			     clipboardContent.isDataFlavorSupported(ErmesSelection.patcherSelectionFlavor));
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

	    pasteItem.setEnabled((flavors != null) &&
				 clipboardContent.isDataFlavorSupported(DataFlavor.stringFlavor));
	  }
	else
	  {
	    if (ErmesSelection.patcherSelection.isEmpty())
	      {
		// Empty selection

		cutItem.setEnabled(false);
		copyItem.setEnabled(false);
		duplicateItem.setEnabled(false);
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
		alignMenu.setEnabled(true);
		toFrontMenuItem.setEnabled(true);
		toBackMenuItem.setEnabled(true);
	      }

	    pasteItem.setEnabled((flavors != null) &&
				 clipboardContent.isDataFlavorSupported(ErmesSelection.patcherSelectionFlavor));
	  }

	selectAllItem.setEnabled(true);	
      }
  }
}







