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

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;

import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;
import javax.swing.undo.*;

/**
 * A TableDataModel associated to an FtsIntegerVector.
 * @see FtsIntegerVector */
public class TableRemoteData implements TableDataModel, ClipableData, ClipboardOwner {
  
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
   * returns the maximum vertical range of the table */
  public int getVerticalSize()
  {
    return Math.abs(max()-min());
  }

  /**
   * Utility to find the maximum (signed) value in the table */
  public int max()
  {
    int max = itsData.getValues()[0];

    for (int i = 0; i < getSize(); i++)
      {
	if (itsData.getValues()[i] > max) max = itsData.getValues()[i];
      }
    
    return max;
  }

  /**
   * Returns the minimum (signed) value in the table */
  public int min()
  {
    int min = itsData.getValues()[0];

    for (int i = 0; i < getSize(); i++)
      {
	if (itsData.getValues()[i] < min) min = itsData.getValues()[i];
      }
    
    return min;
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
	buffer[i-start] = (int)(y1-Math.abs(i-start)*coeff + 0.5);
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

  /**
   * ClipableData interface */
  public void cut()
  {
  }
  
  /**
   * ClipableData interface */
  public void copy()
  {
    if (TableSelection.getCurrent().getModel() != this) return;

    TableSelection.getCurrent().prepareACopy();
    MaxApplication.systemClipboard.setContents(TableSelection.getCurrent(), this);
  }
  
  /**
   * ClipableData interface */
  public void paste()
  {
    int toPaste[] = null;
    TableSelection.TableClip tc = null;

    // check if we can paste:
    if (TableSelection.getCurrent().getModel() != this )
      return;

    Transferable clipboardContent = MaxApplication.systemClipboard.getContents(this);

    if (clipboardContent != null && clipboardContent.isDataFlavorSupported(TableSelection.tableSelection))
      {
	try {
	  tc = (TableSelection.TableClip) clipboardContent.getTransferData(TableSelection.tableSelection);
	  toPaste = tc.content;
	} catch (UnsupportedFlavorException ufe)
	  {
	    // this should never happen...
	    System.err.println("Clipboard error in paste: content does not support "+TableSelection.tableSelection.getHumanPresentableName());
	  } 
	catch (IOException ioe)
	  {
	    System.err.println("Clipboard error in paste: content is no more an "+TableSelection.tableSelection.getHumanPresentableName());
	  }

      }

    if (toPaste != null)
      {
	
	beginUpdate();  //the paste is undoable

	int pointOfInsertion = NOWHERE;
	int numberOfPoints = 0;

	if (!TableSelection.getCurrent().isSelectionEmpty())
	  {
	    pointOfInsertion = TableSelection.getCurrent().getFirstSelected();
	    numberOfPoints = TableSelection.getCurrent().getLastSelected()-pointOfInsertion+1;
	  }
	else if (TableSelection.getCurrent().getCaretPosition() != TableSelection.NO_CARET)
	  {
	    pointOfInsertion = TableSelection.getCurrent().getCaretPosition();
	    numberOfPoints = tc.lenght;
	  }
	
	if (pointOfInsertion != NOWHERE && numberOfPoints > 0)
	  {
	    setValues(toPaste, pointOfInsertion, numberOfPoints);
	    TableSelection.getCurrent().deselectAll();
	  }
	endUpdate();
      }
  }
  
  /** ClipboardOwner interface */

  public void lostOwnership(Clipboard clipboard,
			    Transferable contents)
  {
  }

  //--- Fields
  FtsIntegerVector itsData;
  MaxVector listeners;
  static int buffer[];
  static final int NOWHERE = -1;
}



