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

package ircam.jmax;

import javax.swing.*;
import javax.swing.table.*;

import java.util.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

/**
 * A table model used to represent the content of an objectset of error objects
 * in a JTable. 
 */
class ConfigTableModel extends AbstractTableModel 
{
  
  ConfigTableModel()
  {
    super();
  }

  public void setFtsPackage(FtsPackage pkg)
  {
    ftsPackage = pkg;
  }

  /**
   * The number of columns in this model */
  public int getColumnCount() 
  { 
    return 2;
  }
  
  public Class getColumnClass(int col)
  {
    return String.class;
  }

  public boolean isCellEditable(int row, int col)
  {
    return true;
  }

  public String getColumnName(int col)
  {
    if(col == 0)
      return "property";
    else 
      return "value";
  }

  public int getRowCount() { 
    return size; 
  }

  public void addRow()
  {
    size++;    
    if(size > rows)
      {
	Object[][] temp = new Object[size+5][2];
	for(int i = 0; i < size-1; i++)
	  {
	    temp[i][0] = data[i][0];
	    temp[i][1] = data[i][1];
	  }
	data = temp;
	rows = size+5;
      }
    fireTableDataChanged();
  }

  public void addRow(Object v1, Object v2)
  {
    addRow();
    data[size-1][0] = v1;
    data[size-1][1] = v2;
  }

  public void removeRow(int rowId)
  {
    if(size > 0)
      {
	size--;    
	if(rowId >= 0)
	  {
	    for(int i = rowId; i < size; i++)
	      {
		data[i][0] = data[i+1][0];
		data[i][1] = data[i+1][1];
	      }
	  }
	data[size][0] = null;
	data[size][1] = null;

	fireTableDataChanged();
      }
  }

  public void save()
  {
    if( ftsPackage != null)
      ftsPackage.save();
  }

  public Object getValueAt(int row, int col) 
  { 
    if(row > size) return null;
    else
      return data[row][col];
  }
  public void setValueAt(Object value, int row, int col) 
  {
    if(row > size) return;

    data[row][col] = value;
    fireTableCellUpdated(row, col);
    
    if( ftsPackage != null)
      {
	if(( col==1) && ( data[row][0] != null) && ( data[row][1] != null))
	  ftsPackage.set( (String)data[row][0], (String)data[row][1]);
      }
  }

  int size = 0;
  int rows = 0;
  Object data[][];
  FtsPackage ftsPackage;
}





