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

package ircam.jmax.editors.explode.menus;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;

import ircam.jmax.editors.explode.*;
import ircam.jmax.editors.explode.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;


public class EditMenu extends EditorMenu
{
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

    undoItem      = add(Actions.undoAction, "Undo", Event.CTRL_MASK, KeyEvent.VK_Z);
    redoItem      = add(Actions.redoAction, "Redo", Event.CTRL_MASK, KeyEvent.VK_R);

    addSeparator();
    
    cutItem       = add(Actions.cutAction, "Cut", Event.CTRL_MASK, KeyEvent.VK_X);
    copyItem      = add(Actions.copyAction, "Copy", Event.CTRL_MASK, KeyEvent.VK_C);
    pasteItem     = add(Actions.pasteAction, "Paste", Event.CTRL_MASK, KeyEvent.VK_V);
    duplicateItem = add(Actions.duplicateAction, "Duplicate", Event.CTRL_MASK, KeyEvent.VK_D);
  }

  public void updateMenu()
  {
    Transferable clipboardContent = JMaxApplication.getSystemClipboard().getContents(this);
    DataFlavor[] flavors = clipboardContent.getTransferDataFlavors();

    if(ExplodeSelection.getCurrent().isSelectionEmpty())
      {
	//Empty selection	
	cutItem.setEnabled(false);
	copyItem.setEnabled(false);
	duplicateItem.setEnabled(false);
      }
    else 
      {
	// Object selection
	cutItem.setEnabled(true);
	copyItem.setEnabled(true);
	duplicateItem.setEnabled(true);
      }
    pasteItem.setEnabled((flavors != null) &&
    		  clipboardContent.isDataFlavorSupported(ExplodeDataFlavor.getInstance()));
  }
}





