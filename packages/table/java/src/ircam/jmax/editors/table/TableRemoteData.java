
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
  
  //--- Fields
  FtsIntegerVector itsData;
  MaxVector listeners;
}



