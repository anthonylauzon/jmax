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
// Authors: Francois Dechelle, Norbert Schnell.
// 

package ircam.jmax.editors.patcher;

// import javax.swing.*;
import javax.swing.ListModel;

// import javax.swing.table.*;
import javax.swing.table.AbstractTableModel;

import java.util.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

/**
 * A table model used to represent the content of an objectset of error objects
 * in a JTable. 
 */
class RuntimeErrorsTableModel extends AbstractTableModel implements ToolTableModel
{
  
  RuntimeErrorsTableModel(ListModel list)
  {
    super();
    this.listModel = list;
  }

  /**
   * The number of columns in this model */
  public int getColumnCount() 
  { 
      return 3;
  }
  
  public Class getColumnClass(int col)
  {
      if(col==3)
	  return Integer.class;
      else
	  return String.class;
  }

  public boolean isCellEditable(int row, int col)
  {
    return false;
  }

  public String getColumnName(int col)
  {
      if(col == 0)
	  return "object";
      else if(col == 1)
	  return "error description";
      else
	  return "count";
  }

  public int getRowCount() { 
    return listModel.getSize(); 
  }
  
  public Object getValueAt(int row, int col) 
  { 
      RuntimeError err = (RuntimeError)listModel.getElementAt(row);
      if(err==null) return " ";

      if(col == 0)
	  return err.getObject().getDescription();
      else 
	  if(col == 1)
	      return err.getDescription();    
	  else
	      return new Integer(err.getCount());
  }
  
  public ListModel getListModel()
  {
    return listModel;
  }

  //--- Fields
  ListModel listModel;
}


