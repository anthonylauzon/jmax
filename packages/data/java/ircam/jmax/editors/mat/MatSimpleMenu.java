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

public class MatSimpleMenu extends EditorMenu implements ListSelectionListener
{
  EditorContainer container;
  EditorAction deleteAction, deleteColAction;
  MatDataModel model;
  JTable table;
  int selRowIndex = -1;
  int selRowSize = 0;
  
  public MatSimpleMenu(EditorContainer container, String type)
  {
    super(type);

    this.container = container;
    model = ((MatPanel)container.getEditor()).getMatModel();
    table = ((MatPanel)container.getEditor()).getTable();
    ((MatPanel)container.getEditor()).addTableSelectionListener(this);
    
    setHorizontalTextPosition(AbstractButton.LEFT);
    
		add( new EditorAction("Insert Row", "insert row", KeyEvent.VK_R, KeyEvent.VK_R, true){
      public void doAction(EditorContainer container)
      {
        if(selRowSize == 0)
          model.requestAppendRow();
        else
          model.requestInsertRow(selRowIndex+1);
      }
    });
    
    deleteAction = new EditorAction("Delete Selected Rows", "delete rows", KeyEvent.VK_D, KeyEvent.VK_D, true){
      public void doAction(EditorContainer container)
      {
        if( selRowSize > 0)
          model.requestDeleteRows(selRowIndex, selRowSize);
      }
    };
    
    add( deleteAction);
    		
    if(model.canAppendColumn())
    {
      deleteColAction = new EditorAction("Delete Selected Columns", "delete columns", KeyEvent.VK_C, KeyEvent.VK_C, true){
        public void doAction(EditorContainer container)
        {
          if( table.getSelectedColumnCount() > 0)
            model.requestDeleteCols( table.getSelectedColumn(), table.getSelectedColumnCount());
        } 
      };
      add( deleteColAction);
      
      add( new EditorAction("Insert Column", "insert col", KeyEvent.VK_L, KeyEvent.VK_L, true){
        public void doAction(EditorContainer container)
        {
          if( table.getSelectedColumnCount() == 0)
            model.requestAppendColumn();
          else
            model.requestInsertColumn( table.getSelectedColumn()+1);
        }
      });
    }	    
    
    addSeparator();
            
    add( new EditorAction("Close", "close", KeyEvent.VK_C, KeyEvent.VK_W, true){
      public void doAction(EditorContainer container)
      {
        container.getEditor().close(true);
        System.gc();
      }
    });
  }
  
  public void setFont(Font font)
  {
    super.setFont(font);
    
    JMenuItem item;
    for(int i=0; i<getItemCount(); i++)
    {
      item = getItem(i);
      if(item != null)
        item.setFont(font);
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
      selRowSize = 0;
      selRowIndex = -1;
    }
    else
    {
      deleteAction.setEnabled(true);
      selRowIndex = selection.getMinSelectionIndex();
      selRowSize = selection.getMaxSelectionIndex() - selRowIndex + 1;
    }
    if(model.canAppendColumn())
      deleteColAction.setEnabled( (table.getSelectedColumnCount() > 0));
  }
}





