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

package ircam.jmax.editors.table.menus;

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

import ircam.jmax.editors.table.*;
import ircam.jmax.editors.table.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;


public class EditMenu extends EditorMenu
{
  EditorContainer container;
  FtsTableObject ftsObject;

  JMenuItem undoItem;
  JMenuItem redoItem;
  JMenuItem copyItem;
  JMenuItem cutItem;
  JMenuItem insertItem;
  JMenuItem pasteItem;
  JMenuItem refreshItem;
  
  public EditMenu(EditorContainer container)
  {
    super("Edit");

    this.container = container;
    this.ftsObject = (FtsTableObject)((TablePanel)container.getEditor()).getData();

    setHorizontalTextPosition(AbstractButton.LEFT);

    undoItem      = add(Actions.undoAction, "Undo", Event.CTRL_MASK, KeyEvent.VK_Z);
    redoItem      = add(Actions.redoAction, "Redo", Event.CTRL_MASK, KeyEvent.VK_R);

    addSeparator();
    
    cutItem       = add(Actions.cutAction, "Cut", Event.CTRL_MASK, KeyEvent.VK_X);
    copyItem      = add(Actions.copyAction, "Copy", Event.CTRL_MASK, KeyEvent.VK_C);
    pasteItem     = add(Actions.pasteAction, "Paste", Event.CTRL_MASK, KeyEvent.VK_V);
    insertItem    = add(Actions.insertAction, "Insert", Event.CTRL_MASK, KeyEvent.VK_I);

    addSeparator();

    refreshItem   = add(Actions.refreshAction, "Refresh");
  }

  public void updateMenu()
  {
      //Transferable clipboardContent = MaxApplication.systemClipboard.getContents(this);
      //DataFlavor[] flavors = clipboardContent.getTransferDataFlavors();
    if(TableSelection.current.isSelectionEmpty())
      {
	//Empty selection	
	copyItem.setEnabled(false);
	cutItem.setEnabled(false);
      }
    else 
      {
	// Object selection
	copyItem.setEnabled(true);
	cutItem.setEnabled(true);
      }
    /*pasteItem.setEnabled((flavors != null) &&
      clipboardContent.isDataFlavorSupported(TableSelection.tableSelection));
      insertItem.setEnabled((flavors != null) &&
      clipboardContent.isDataFlavorSupported(TableSelection.tableSelection));*/
    
    pasteItem.setEnabled(ftsObject.thereIsACopy());
    insertItem.setEnabled(ftsObject.thereIsACopy());
  }
}




