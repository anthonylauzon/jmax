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

package ircam.jmax.editors.table;

import java.util.Enumeration;

/**
 * The functionalities of the model for the jMax table data. 
 * TableRemoteData implements it.
 */
public interface TableDataModel {

  /**
   * how big is the table?
   */
    public abstract int getSize();
  
  /**
   * Returns the vertical range of this table, that is, 
   * the abs(max-min)  */
    public abstract int getVerticalSize();

  /**
   * Returns the maximum (signed) value in the table */
    public abstract int max();

  /**
   * Returns the minimum (signed) value in the table */
    public abstract int min();

  /**
   * returns a vector of all the values
   */
    public abstract int[] getValues();

  /**
   * returns the value at the given position
   */
    public abstract int getValue(int index);

  /**
   * set a value in the given index
   */
    //public abstract void setValue(int index, int value);

  /**
   * set lenght values form startIndex taking them from the given array 
   */
    //public abstract void setValues(int values[], int startIndex, int lenght);

  /**
   * Sets the range of points between start and end interpolating between the initial
   * and ending values. This operation corresponds to draw a line in the model. */
    public abstract void interpolate(int start, int end, int startValue, int endValue);


  /**
   * requires to be notified when the database changes
   */
    public abstract void addListener(TableDataListener theListener);

  /**
   * removes the listener
   */
    public abstract void removeListener(TableDataListener theListener);
}




