
package ircam.jmax.editors.table;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import java.util.*;
import javax.swing.undo.*;

/**
 * A TableDataModel linked to an FtsIntegerVector.
 * @see FtsIntegerVector */
public class TableRemoteData implements TableDataModel {
  
  /**
   * Constructor */
  public TableRemoteData(FtsIntegerVector theData)
  {
    itsData = theData;

    listeners = new MaxVector();
  }

  /**
   * how big is the table?
   */
  public int getSize()
  {
    return itsData.getSize();
  }


  /**
   * returns the value at the given position
   */
  public int getValue(int index)
  {
    return itsData.getValues()[index];
  }

  /**
   * returns a vector of all the values
   */
  public int[] getValues()
  {
    return itsData.getValues();
  }

  /**
   * set a value in the given index
   */
  public void setValue(int index, int value)
  {
    if (index < 0 || index >= getSize()) return;
    if (itsData.getValues()[index] != value)
      {
	if (isInGroup()) 
	  postEdit(new UndoableValueSet(this, index, itsData.getValues()[index]));
	itsData.getValues()[index] = value;
	
	itsData.changed(index);
	notifyChange(index);
      }
  }

  /**
   * set lenght values form startIndex taking them from the given array 
   */
  public void setValues(int values[], int startIndex, int lenght)
  {

    if (startIndex > getSize()) return;
    if (startIndex < 0) startIndex = 0;
    if (startIndex + lenght > getSize()) lenght = getSize()-startIndex;

    
    for (int i = 0; i < lenght; i++)
      {
	if (itsData.getValues()[startIndex + i] != values[i])
	  {

	    if (isInGroup()) 
	      postEdit(new UndoableValueSet(this, startIndex+i, itsData.getValues()[startIndex+i]));    
	    itsData.getValues()[startIndex+i] = values[i];
	    
	    itsData.changed(startIndex+i);
	  }

      }

    notifyChange(startIndex, startIndex+lenght);
  }

  /**
   * Sets the range of points between start and end interpolating between the initial
   * and ending values. This operation graphically corresponds to a line. */
  public void interpolate(int start, int end, int y1, int y2)
  {
    float coeff;
    if (y1 != y2) 
      coeff = ((float)(y1 - y2))/(end - start);
    else coeff = 0;

    prepareBuffer(end-start+1);

    if (end >= getSize()) 
      end = getSize()-1;
    
    for (int i = start; i < end; i+=1)
      {
	buffer[i-start] = (int)(y1-Math.abs(i-start)*coeff);
      }

    setValues(buffer, start, end-start);
  }

  /**
   * Force the values in FTS to be transmitted in the editor */
  public void forceUpdate()
  {
    itsData.forceUpdate();
  }

  /**
   * tells the model to start an undo section */
  public void beginUpdate()
  {
    itsData.beginUpdate();
  }

  /**
   * posts an undo edit in the buffers */
  public void postEdit(UndoableEdit e)
  {
    itsData.postEdit(e);
  }
  
  /**
   * terminates the undo critical section */
  public void endUpdate()
  {
    itsData.endUpdate();
  }
  
  /**
   * ask to undo the section */
  public void undo()
  {
    itsData.undo();
  }

  /**
   * ask to redo the section */
  public void redo()
  {
    itsData.redo();
  }

  /**
   * are we in the midst of an undoable section? */
  public boolean isInGroup()
  {
    return itsData.isInGroup();
  }

  /**
   * requires to be notified when the database changes
   */
  public void addListener(TableDataListener theListener)
  {
    listeners.addElement(theListener);
  }

  /**
   * removes the listener
   */
  public void removeListener(TableDataListener theListener)
  {
    listeners.removeElement(theListener);
  }

  /**
   * Utility function to call back the listeners of this data model */
  private void notifyChange(int index)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((TableDataListener) e.nextElement()).valueChanged(index);
  }

  /**
   * Utility function to call back the listeners of this data model */
  private void notifyChange(int index1, int index2)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((TableDataListener) e.nextElement()).valueChanged(index1, index2);
  }
  
  /**
   * Utility private function to allocate a buffer used during the interpolate operations.
   * The computation is done in a private vector that is stored in one shot. */
  private static void prepareBuffer(int lenght)
  {
    if (buffer == null || buffer.length < lenght)
      {
	buffer = new int[lenght];
      }
  }

  //--- Fields
  FtsIntegerVector itsData;
  MaxVector listeners;
  static int buffer[];

}



