//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.UndoableData;

import java.util.Enumeration;

/**
 * The functionalities of the model for the jMax table data. 
 * TableRemoteData implements it.
 */
public interface TableDataModel extends UndoableData{

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
  public abstract void setValue(int index, int value);

  /**
   * set lenght values form startIndex taking them from the given array 
   */
  public abstract void setValues(int values[], int startIndex, int lenght);


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



