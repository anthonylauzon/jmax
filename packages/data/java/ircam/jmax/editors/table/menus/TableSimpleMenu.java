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

import ircam.jmax.editors.table.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.toolkit.actions.*;

public class TableSimpleMenu extends EditorMenu
{
  EditorContainer container;

  /*public EditorAction cutAction       	  = new Actions.CutAction();
  public EditorAction copyAction          = new Actions.CopyAction();
  public EditorAction duplicateAction 	  = new Actions.DuplicateAction();*/

  public TableSimpleMenu(EditorContainer container)
  {
    super("Table");

    this.container = container;

    /*setHorizontalTextPosition(AbstractButton.LEFT);
    setMnemonic(KeyEvent.VK_T);
    
    add(Actions.undoAction);
    add(Actions.redoAction);
    addSeparator();
    add(cutAction);
    add(copyAction);
    add(Actions.pasteAction);
    add(duplicateAction);*/

    add( new EditorAction("Inspect ...", "inspect", KeyEvent.VK_C, KeyEvent.VK_I, true){
      public void doAction(EditorContainer container)
      {
        TableInspector.inspect((TableDisplay)((TablePanel)container.getEditor()).getGraphicContext().getGraphicDestination(), container.getFrame(), new Point(100, 100));
      }
    });
    
    addSeparator();
        
    add( new EditorAction("Close", "close", KeyEvent.VK_C, KeyEvent.VK_W, true){
      public void doAction(EditorContainer container)
    {
        container.getEditor().close(true);
        System.gc();
    }
    });
  }

  public void updateMenu()
  {
    /*boolean enable = !((SequenceSelection.getCurrent() == null)||(SequenceSelection.getCurrent().isSelectionEmpty()));
    cutAction.setEnabled(enable);
    copyAction.setEnabled(enable);
    duplicateAction.setEnabled(enable);*/
  }  
}





