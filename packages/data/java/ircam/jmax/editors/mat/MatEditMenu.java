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

package ircam.jmax.editors.mat;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.toolkit.actions.*;

public class MatEditMenu extends EditorMenu implements ListSelectionListener
{
  EditorContainer container;
  EditorAction deleteAction;
  MatDataModel model;
  int selIndex = -1;
  int selSize = 0;
  
  public MatEditMenu(EditorContainer container)
  {
    super("Edit");

    this.container = container;
    model = ((MatPanel)container.getEditor()).getMatModel();
    
    ((MatPanel)container.getEditor()).addTableSelectionListener(this);
    
    setHorizontalTextPosition(AbstractButton.LEFT);
    
		add( new EditorAction("Insert Row", "insert row", KeyEvent.VK_R, KeyEvent.VK_R, true){
      public void doAction(EditorContainer container)
      {
        if(selSize == 0)
          model.requestAppendRow();
        else
          model.requestInsertRow(selIndex+1);        
      }
    });
    
    deleteAction = new EditorAction("Delete Selected Rows", "delete rows", KeyEvent.VK_D, KeyEvent.VK_D, true){
      public void doAction(EditorContainer container)
      {
        if( selSize > 0)
          model.requestDeleteRows(selIndex, selSize);
      }
    };
    
    add( deleteAction);
    		
    if(model.canAppendColumn())
    {
      add( new EditorAction("Append Column", "append col", KeyEvent.VK_L, KeyEvent.VK_L, true){
        public void doAction(EditorContainer container)
        {
          model.requestAppendColumn();
        }
      });
    }	    
  }
  
  /* ListSelectionListener interface */
  public void valueChanged(ListSelectionEvent e)
  {
    ListSelectionModel selection = (ListSelectionModel) e.getSource();
    if(selection.getValueIsAdjusting()) return;
    
    if(selection.isSelectionEmpty())
    {
      deleteAction.setEnabled(false);
      selSize = 0;
      selIndex = -1;
    }
    else
    {
      deleteAction.setEnabled(true);
      selIndex = selection.getMinSelectionIndex();
      selSize = selection.getMaxSelectionIndex() - selIndex + 1;
    }
  }
}




