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

import javax.swing.*;
import javax.swing.table.*;
import javax.swing.undo.*;
import java.util.*;
import java.lang.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;


class MatTableModel extends AbstractTableModel
{
  MatTableModel(MatDataModel model)
 {
    super();
    this.model = model;
 }

  public int getColumnCount() 
{ 
    return model.getColumns();
}

int editedRowIndex, editedColumnIndex;
java.lang.Object editedValue = null;
  
public void setValueAt(Object aValue, int rowIndex, int columnIndex) 
{	 
  editedRowIndex = rowIndex;
  editedColumnIndex = columnIndex;
  editedValue = aValue;
  
  SwingUtilities.invokeLater(new Runnable() {
    public void run()
    { 
      if( editedValue != null && !(editedValue instanceof FtsObject))
      {
        Object obj;
        try{
          obj = Integer.valueOf((String)editedValue);
        }
        catch(NumberFormatException e)
        {
          try{
            obj = Double.valueOf((String)editedValue);
          }
          catch(NumberFormatException e1)
          {
            obj = editedValue;
          }
        } 
        model.requestSetValue( obj, editedRowIndex, editedColumnIndex);
      }
    }
  });
}

public boolean isCellEditable(int row, int col)
{
  return true;
}

public String getColumnName(int col)
{
  return model.getColumnName(col);
}
public int getRowCount() { 
  return model.getRows();
}

public Object getValueAt(int row, int col) 
{ 
  if(row < getRowCount() && col < getColumnCount())
    return  model.getValueAt(row, col);
  else
    return null;
}

public MatDataModel getMatDataModel()
{
	return model;
}

//--- Fields
MatDataModel model;
}


