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

package ircam.jmax.editors.table.menus;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.table.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.toolkit.actions.*;


public class EditMenu extends EditorMenu
{
  EditorContainer container;
  FtsTableObject ftsObject;

  public EditorAction undoAction = new Actions.UndoAction();
  public EditorAction redoAction = new Actions.RedoAction();
  public EditorAction copyAction = new Actions.CopyAction();
  public EditorAction cutAction = new Actions.CutAction();
  public EditorAction insertAction = new Actions.InsertAction();
  public EditorAction pasteAction = new Actions.PasteAction();
  public EditorAction refreshAction = new Actions.RefreshAction();
  
  public EditMenu(EditorContainer container)
  {
    super("Edit");

    this.container = container;
    this.ftsObject = (FtsTableObject)((TablePanel)container.getEditor()).getData();
    setHorizontalTextPosition(AbstractButton.LEFT);
    setMnemonic(KeyEvent.VK_E);

    add(undoAction);
    add(redoAction);

    addSeparator();
    
    add(cutAction);
    add(copyAction);
    add(pasteAction);
    add(insertAction);
    
    addSeparator();

    add(refreshAction);
    
    pasteAction.setEnabled(ftsObject.thereIsACopy());    
  }

  public void updateMenu()
  {
    if(TableSelection.current.isSelectionEmpty())
    {
      //Empty selection
      copyAction.setEnabled(false);
      cutAction.setEnabled(false);
    }
    else
    {
      // Object selection
      copyAction.setEnabled(true);
      cutAction.setEnabled(true);
    }

    pasteAction.setEnabled(ftsObject.thereIsACopy());
    insertAction.setEnabled(ftsObject.thereIsACopy());
  }
}




