
package ircam.jmax.editors.table;

import ircam.jmax.fts.*;
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
   * requires to be notified when the database changes
   */
  public abstract void addListener(TableDataListener theListener);

  /**
   * removes the listener
   */
  public abstract void removeListener(TableDataListener theListener);


}



