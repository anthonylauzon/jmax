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

package ircam.jmax.editors.sequence.track;

import javax.swing.*;
import javax.swing.table.*;
import javax.swing.undo.*;

import java.util.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.*;

/**
* A table model used to represent the content of an track object
 * in a JTable. TrackTableModel is a swing TableModel
 * built on top of an TableDataModel object. 
 */
class TrackTableModel extends AbstractTableModel{
  
  /**
	* An trackTableModel is created starting from an TrackDataModel (es. an
																																	 * FtsTrackObject) */
  TrackTableModel(TrackDataModel model)
{
    super();
    this.model = model;
}

/**
* The number of columns in this model */
public int getColumnCount() 
{ 
	return 1 + model.getPropertyCount();
}

/**
* The class representing a generic entry in (a column of) the table */
public Class getColumnClass(int col)
{
	int type;
	if(col == 0)
    return Double.class;
	else	      
		return model.getPropertyType(col-1);
}

/**
* SetValue method: invoked by the cellEditor, sets the given value
 * in the Explode. Row is the event number, column is the field to change. 
 * @see WholeNumberField*/
public void setValueAt(java.lang.Object aValue, int rowIndex, int columnIndex) 
{		
	Event event = model.getEventAt(rowIndex);
	
	if (model instanceof UndoableData) //can't make assumptions...
		((UndoableData) model).beginUpdate(); 
	
	if(columnIndex == 0)
		event.move(((Double) aValue).doubleValue());
	else
		if( aValue == null || (aValue instanceof String && ((String)aValue).equals("")))
			event.unsetProperty( getColumnName(columnIndex));
	else 
		event.setProperty( getColumnName(columnIndex), aValue);
	
	if (model instanceof UndoableData)
		((UndoableData) model).endUpdate();
}

/**
* Every field in an explode is editable, except the event number */
public boolean isCellEditable(int row, int col)
{
  return true;
}

/**
* Returns the Name of the given column */
public String getColumnName(int col)
{
	String name;
	if(col == 0)
    return "time";
  else
  {
    int i = 1;
    for(Enumeration e = model.getPropertyNames(); e.hasMoreElements();)
    {
      name = (String)e.nextElement();
      if(i==col)
        return name;
      i++;
    }
  }
	/*}*/
	return "";
}

/**
* How many events in the database? */
public int getRowCount() { 
	return model.length(); 
}

/**
* Returns the value of the given field of the given Event */
public Object getValueAt(int row, int col)
{ 
		Event temp = model.getEventAt(row);    
		
		if(col == 0)
			return new Float(temp.getTime());
		else
	  {
	    Object val = temp.getValue().getPropertyValues()[col-1];
      
	    if( val != EventValue.UNKNOWN_PROPERTY) 
	      return val;
	    else return null;
	  }
}

/**
* Method to access the TrackData this table refers to */
public TrackDataModel getTrackDataModel()
{
	return model;
}

//--- Fields
TrackDataModel model;
}


