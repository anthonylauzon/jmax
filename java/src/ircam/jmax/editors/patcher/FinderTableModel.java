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
class FinderTableModel extends AbstractTableModel implements ToolTableModel
{
  
  FinderTableModel(FtsObjectSet set)
  {
    super();
    this.objectSet = set;
  }

  /**
   * The number of columns in this model */
  public int getColumnCount() 
  { 
      return 1;
  }
  
  public Class getColumnClass(int col)
  {
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
      else 
	  return "";
  }

  public int getRowCount() { 
    return objectSet.getSize(); 
  }
  
  public Object getValueAt(int row, int col) 
  { 
      FtsGraphicObject obj = (FtsGraphicObject)objectSet.getElementAt(row);
      if(obj==null) return " ";

      String val = obj.getDescription();
      val.trim();
      if(val.equals("")) val = obj.getComment();

      return val;
  }
  
  public ListModel getListModel()
  {
      return objectSet;
  }
  
  //--- Fields
  FtsObjectSet objectSet;
}


