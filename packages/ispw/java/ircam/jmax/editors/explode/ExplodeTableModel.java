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

package ircam.jmax.editors.explode;

import javax.swing.*;
import javax.swing.table.*;
import javax.swing.undo.*;

import ircam.jmax.toolkit.*;

/**
 * A table model used to represent the content of an explode object
 * in a JTable. ExplodeTableModel is a swing TableModel
 * built on top of an ExplodeDataModel object. Note that the number of columns
 * is fixed and depends on the number of fields of a generic explode event 
 * in FTS.
 */
class ExplodeTableModel extends AbstractTableModel{
  
  /**
   * An explodeTableModel is created starting from an ExplodeDataModel (es. an
   * ExplodeRemoteData) */
  ExplodeTableModel(ExplodeDataModel explode)
  {
    super();
    this.explode = explode;
  }

  /**
   * The number of columns in this model */
  public int getColumnCount() 
  { 
    return NO_OF_FIELDS;
  }
  
  /**
   * The class representing a generic entry in (a column of) the table */
  public Class getColumnClass(int col)
  {
    return Integer.class;
  }

  /**
   * SetValue method: invoked by the cellEditor, sets the given value
   * in the Explode. Row is the event number, column is the field to change. 
   * @see WholeNumberField*/
  public void setValueAt(java.lang.Object aValue, int rowIndex,
                 int columnIndex) 
  {
    int value = ((Integer) aValue).intValue();
    ScrEvent event = explode.getEventAt(rowIndex);

    if (explode instanceof UndoableData) //can't make assumptions...
      ((UndoableData) explode).beginUpdate(); 
    switch(columnIndex) 
	{
	case EVTNO_COLUMN:
	  return; //should never happen, since column EVTNO_COLUMN is NOT editable
	case TIME_COLUMN:
	  
	  event.move(value);
	  // changing the time means moving the event in the db.
	  // This will eventually result in a change of its row (ExplodeTablePanel is listening at this kind of events)
	  break;
	  
	case PITCH_COLUMN:
	  event.setPitch(value);
	  break;

	case DURATION_COLUMN:
	  event.setDuration(value);
	  break;

	case VELOCITY_COLUMN:
	  event.setVelocity(value);
	  break;

	case CHANNEL_COLUMN:
	  event.setChannel(value);
	  break;

	default: break;
	}
    
    if (explode instanceof UndoableData) //can't make assumptions...
      ((UndoableData) explode).endUpdate();

  }

  /**
   * Every field in an explode is editable, except the event number */
  public boolean isCellEditable(int row, int col)
  {
    return col != EVTNO_COLUMN;
  }

  /**
   * Returns the Name of the given column */
  public String getColumnName(int col)
  {
    switch(col) 
	{
	case EVTNO_COLUMN:
	  return "Evt. no";
	case TIME_COLUMN:
	  return "Start time";

	case PITCH_COLUMN:
	  return "Pitch";

	case DURATION_COLUMN:
	  return "Duration";

	case VELOCITY_COLUMN:
	  return "Velocity";

	case CHANNEL_COLUMN:
	  return "Channel";

	}
    return "";
  }

  /**
   * How many events in the database? */
  public int getRowCount() { 
    return explode.length(); 
  }
  
  /**
   * Returns the value of the given field of the given ScrEvent */
  public Object getValueAt(int row, int col) { 

    if (col == EVTNO_COLUMN)
      return new Integer(row);
    else {
      ScrEvent temp = explode.getEventAt(row);
      int value;

      switch(col) 
	{
	case TIME_COLUMN:
	  value = temp.getTime();
	  break;
	case PITCH_COLUMN:
	  value = temp.getPitch();
	  break;
	case DURATION_COLUMN:
	  value = temp.getDuration();
	  break;
	case VELOCITY_COLUMN:
	  value = temp.getVelocity();
	  break;
	case CHANNEL_COLUMN:
	  value = temp.getChannel();
	  break;
	default:
	  value = 0;
	  break;
	}
      return new Integer(value);
    }
  }
  
  /**
   * Method to access the ExplodeData this table refers to */
  public ExplodeDataModel getExplodeDataModel()
  {
    return explode;
  }

  //--- Fields
  ExplodeDataModel explode;

  public static final int NO_OF_FIELDS = 6; //the number 

  public static final int EVTNO_COLUMN = 0; //first column contains the event number
  public static final int TIME_COLUMN = 1;
  public static final int PITCH_COLUMN = 2;
  public static final int DURATION_COLUMN = 3;
  public static final int VELOCITY_COLUMN = 4;
  public static final int CHANNEL_COLUMN = 5;
}


