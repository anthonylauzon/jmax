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

package ircam.jmax.editors.sequence.menus;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;


public class EditMenu extends EditorMenu
{
  class EditMenuListener implements MenuListener
  {
    public void menuSelected(MenuEvent e)
    {
      updateMenu();
    }

    public void menuDeselected(MenuEvent e)
    {
    }

    public void menuCanceled(MenuEvent e)
    {
    }
  }

  EditorContainer container;

  JMenuItem undoItem;
  JMenuItem redoItem;
  JMenuItem cutItem;
  JMenuItem copyItem;
  JMenuItem pasteItem;
  JMenuItem duplicateItem;
  
  public EditMenu(EditorContainer container)
  {
    super("Edit");

    this.container = container;

    setHorizontalTextPosition(AbstractButton.LEFT);

    undoItem      = add(Actions.undoAction, "Undo", java.awt.Event.CTRL_MASK, KeyEvent.VK_Z);
    redoItem      = add(Actions.redoAction, "Redo", java.awt.Event.CTRL_MASK, KeyEvent.VK_R);

    addSeparator();
    
    cutItem       = add(Actions.cutAction, "Cut", java.awt.Event.CTRL_MASK, KeyEvent.VK_X);
    copyItem      = add(Actions.copyAction, "Copy", java.awt.Event.CTRL_MASK, KeyEvent.VK_C);
    pasteItem     = add(Actions.pasteAction, "Paste", java.awt.Event.CTRL_MASK, KeyEvent.VK_V);
    duplicateItem = add(Actions.duplicateAction, "Duplicate", java.awt.Event.CTRL_MASK, KeyEvent.VK_D);

    addMenuListener(new EditMenuListener());
  }

  private void updateMenu()
  {
    Transferable clipboardContent = MaxApplication.systemClipboard.getContents(this);
    DataFlavor[] flavors = clipboardContent.getTransferDataFlavors();

    if(SequenceSelection.getCurrent().isSelectionEmpty())
      {
	//Empty selection	
	cutItem.setEnabled(false);
	copyItem.setEnabled(false);
	duplicateItem.setEnabled(false);
      }
    else /*if(ErmesSelection.patcherSelection.getOwner() == container)*/
      {
	// Object selection
	cutItem.setEnabled(true);
	copyItem.setEnabled(true);
	duplicateItem.setEnabled(true);
      }
    pasteItem.setEnabled((flavors != null) &&
			 clipboardContent.isDataFlavorSupported(SequenceDataFlavor.getInstance()));
  }
}





