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

package ircam.jmax.editors.bpf;

import javax.swing.*;
import javax.swing.table.*;
import javax.swing.undo.*;

import java.util.*;
import ircam.jmax.toolkit.*;

/**
 * A table model used to represent the content of an explode object
 * in a JTable. ExplodeTableModel is a swing TableModel
 * built on top of an ExplodeDataModel object. Note that the number of columns
 * is fixed and depends on the number of fields of a generic explode event 
 * in FTS.
 */
class BpfTableModel extends AbstractTableModel{
  
  /**
   * An trackTableModel is created starting from an TrackDataModel (es. an
   * FtsTrackObject) */
  BpfTableModel(BpfDataModel model)
  {
    super();
    this.model = model;
  }

  /**
   * The number of columns in this model */
  public int getColumnCount() 
  { 
      return 3;
  }
  
  /**
   * The class representing a generic entry in (a column of) the table */
  public Class getColumnClass(int col)
  {
      int type;
      if(col == 0)
	  return Integer.class;
      else 
	  return Float.class;
  }

  /**
   * SetValue method: invoked by the cellEditor, sets the given value
   * in the Explode. Row is the event number, column is the field to change. 
   * @see WholeNumberField*/
  public void setValueAt(java.lang.Object aValue, int rowIndex, int columnIndex) 
  {
      if(columnIndex == 0) return;
      
      BpfPoint point = model.getPointAt(rowIndex);

      if (model instanceof UndoableData) //can't make assumptions...
	  ((UndoableData) model).beginUpdate(); 
      
      if(columnIndex == 1)
	  ((FtsBpfObject)model).requestSetPoint(rowIndex, ((Float) aValue).floatValue(), point.getValue());      
      else
	  ((FtsBpfObject)model).requestSetPoint(rowIndex, point.getTime(), ((Float) aValue).floatValue());      

    if (model instanceof UndoableData)
	((UndoableData) model).endUpdate();

  }

  /**
   * Every field in an explode is editable, except the event number */
  public boolean isCellEditable(int row, int col)
  {
    return col != 0;
  }

  /**
   * Returns the Name of the given column */
  public String getColumnName(int col)
  {
      String name;
      
      switch(col)
	  {
	  case 0:
	      return "point no";
	  case 1:
	       return "time";
	  case 2:
	      return "value";
	  default:
	      return "";    
	  }
  }

  /**
   * How many events in the database? */
  public int getRowCount() { 
      return model.length(); 
  }
  
  /**
   * Returns the value of the given field of the given Event */
  public Object getValueAt(int row, int col) { 

    if(col == 0)
      return new Integer(row);
    else 
	{
	    BpfPoint temp = model.getPointAt(row);    
	    
	    if(col == 1)
		return new Float(temp.getTime());
	    else
		return new Float(temp.getValue());
	}
  }
  
  /**
   * Method to access the TrackData this table refers to */
  public BpfDataModel getDataModel()
  {
    return model;
  }

  //--- Fields
  BpfDataModel model;
}


