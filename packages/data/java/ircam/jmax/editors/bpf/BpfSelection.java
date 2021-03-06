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

package ircam.jmax.editors.bpf;

import java.util.*;
import javax.swing.*;
import java.awt.datatransfer.*;
import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import java.lang.reflect.*;

/**
* The Sequence selection. This class is implemented on the base of the
 * Swing's ListSelectionModel, so that it can be used unchanged in
 * the JTable representation of the score.
 * Even if every Sequence track have its own selection, just one of them
 * is active (current) at a time. The active selection is set via the setCurrent method.
 * A SequenceSelection has an owner (a SelectionOwner implementor): this 
 * Owner will be called back when the selection become active or disactive.
 * A Partition Event Renderer could use the ownership information to draw
 * the selected event differently (selected-active / selected-non active...)
 * @see SequenceTablePanel */
public class BpfSelection extends DefaultListSelectionModel implements BpfDataListener{
  
  
  //--- Fields
  //private static BpfSelection current;
  
  public DataFlavor flavors[];
  
  private MaxVector temp = new MaxVector();
  
  protected  BpfDataModel itsModel;
  protected BpfPoint lastSelectedPoint = null;
  protected BpfGraphicContext gc;
  
  // Implementation notes: 
  // The ListSelectionModel, and then the SequenceSelection, 
  // are based on indexes, while the identity
  // of an sequence event is only partially related to its index 
  // (infact, it can be moved around in the DB).
  // This imposes some extra-care while dealing with moving and deleting 
  // operation.
  // See also the notes in the objectMoved call.
  public BpfSelection(BpfDataModel model, BpfGraphicContext gc) 
  {
    itsModel = model;
    this.gc = gc;
    setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION) ;
    model.addBpfListener(this);
  }
  
  public BpfSelection(){}
  
  public void addSelectionInterval(int index1, int index2)
  {
    super.addSelectionInterval(index1, index2);
    gc.displaySelectionInfo();
  }
  
  /**
    * Returns the SequenceDataModel this selection refers to */
  public BpfDataModel getModel()
  {
    return itsModel;
  }
  
  /** select the given object 
    */
  public void select(Object obj)
  {
    int index = itsModel.indexOf((BpfPoint) obj);
    
    if (!isSelectedIndex(index)) 
    {
	    lastSelectedPoint = (BpfPoint)obj;
	    addSelectionInterval(index, index);
    }
  }
  
  public void select(int index)
  {
    if (!isSelectedIndex(index)) 
    {
	    lastSelectedPoint = itsModel.getPointAt(index);
	    addSelectionInterval(index, index);
    }
  }
  
  /** Select the given enumeration of objects.
    * When possible, use this method instead of
    * selecting single objects. 
    */
  public void select(Enumeration e)
  {
    if(!e.hasMoreElements()) return;
    
    setValueIsAdjusting(true);
    
    int index;
    int min = itsModel.length()-1;
    int max = 0;
    while(e.hasMoreElements())
    {
	    index = itsModel.indexOf((BpfPoint) e.nextElement()); 
	    if(index > max) max = index;
	    if(index < min) min = index;
    }
    
    addSelectionInterval(min, max);
    lastSelectedPoint = itsModel.getPointAt(max);
    
    setValueIsAdjusting(false);
  }
  
  /** remove the given object from the selection
    */
  public void deSelect(Object obj)
  {
    int index = itsModel.indexOf((BpfPoint) obj);
    if(obj == lastSelectedPoint) lastSelectedPoint = null;
    removeSelectionInterval(index, index);
  }
  
  /** remove the given enumeration of objects from the selection
    * When possible, use this method instead of
    * deselecting single objects.    */
  public void deSelect(Enumeration e)
  {
    while(e.hasMoreElements())
    {
      deSelect(e.nextElement());
    }
  }
  
  public BpfPoint getFirstInSelection()
  {
    return itsModel.getPointAt(getMinSelectionIndex());
  }
  public BpfPoint getLastInSelection()
  {
    return itsModel.getPointAt(getMaxSelectionIndex());
  }
  
  public float getMaxValueInSelection()
  {
    float value;
    float max = itsModel.getMinimumValue();
    for (int i = getMinSelectionIndex(); i <= getMaxSelectionIndex(); i++)
    {
      value = itsModel.getPointAt(i).getValue();
      if(value>max) max=value;
    }
    return max;
  }
  public float getMinValueInSelection()
  {
    float value;
    float min = itsModel.getMaximumValue();
    for (int i = getMinSelectionIndex(); i <= getMaxSelectionIndex(); i++)
    {
      value = itsModel.getPointAt(i).getValue();
      if(value<min) min=value;
    }
    return min;
  }
  /** returns true if the object is currently selected
    */
  public boolean isInSelection(Object obj)
  {
    int index = itsModel.indexOf((BpfPoint) obj);
    
    return isSelectedIndex(index);
  }
  
  
  /** Returns an enumeration of all the selected OBJECTS (not indexes).
    * The enumeration is "delete tollerant": one can remove objects
    * in a loop based on the result of this method.
    */
  public Enumeration getSelected()
  {
    // NOTE: a copy must be made because the caller can use the result
    // to perform operations that delete or move events
    temp.removeAllElements();
    for (int i = getMinSelectionIndex(); i <= getMaxSelectionIndex(); i++)
      if (isSelectedIndex(i)) temp.addElement(itsModel.getPointAt(i));
    
    return temp.elements(); 
  }
  
  
  /** Returns the number of objects in the selection
    */
  public  int size()
  {
    // unefficient! Alternative solutions welcome.
    int count = 0;
    
    for (int i = getMinSelectionIndex(); i <= getMaxSelectionIndex(); i++) 
	  {
      if (isSelectedIndex(i))
        count++;
	  }
    
    return count;
    //return (getMaxSelectionIndex() -  getMinSelectionIndex() + 1);
  }
  
  public BpfPoint getLastSelectedPoint()
  {
    return lastSelectedPoint;
  }
  public void setLastSelectedPoint(BpfPoint last)
  {
    lastSelectedPoint = last;
  }
  
  /** selects all the objects. 
    */
  public  void selectAll()
  {
    addSelectionInterval(0, itsModel.length()-1);
  }
  
  /** deselects all the objects currently selected
    */
  public  void deselectAll()
  {
    lastSelectedPoint = null;
    clearSelection();
  }
  
  /** BpfDataListener interface*/
  
  public void pointAdded(int index) 
  {
    select(index);    
  }
  /**
    * BpfDataListener interface: keep the index-based selection consistent. */
  public void pointChanged(int oldIndex, int newIndex, float newTime, float newValue)
  {
    // NOTE: oldIndex is the index where the object WAS before moving,
    // newIndex is the index where the object IS (after rearranging the vector).
    // When such a movement occurs, also all the objects whose indexes are 
    // between oldIndex and newIndex are shifted by one.
    // The purpose of this routine is to keep the selected/unselected 
    // status of these indexes coherent with the shifts.
    
    boolean wasSelected = isSelectedIndex(oldIndex);
    
    if (oldIndex == newIndex) 
      return; //no changes in the object's order
    
    if (oldIndex < newIndex)
    {
      for (int i = oldIndex; i<newIndex; i++)
      {
        if (isSelectedIndex(i+1))
          addSelectionInterval(i, i);
        else
          removeSelectionInterval(i,i);
      }
    }
    else 
    {
      for (int i=oldIndex; i>newIndex; i--)
      {
        if (isSelectedIndex(i-1))
          addSelectionInterval(i, i);
        else
          removeSelectionInterval(i,i);
      } 
      
    }
    
    if (wasSelected && !isSelectedIndex(newIndex)) 
      addSelectionInterval(newIndex, newIndex);
    
  }
  
  public void pointsChanged(){}
  
  /** BpfDataListener interface */
  public void pointsDeleted( int index, int size) 
  {
    for(int i =0; i<size;i++)
    {
      if(isSelectedIndex(index + i))
		    removeSelectionInterval(i,i);
      else
		    if(index + i < getMinSelectionIndex())
		      setSelectionInterval(getMinSelectionIndex()-1, getMaxSelectionIndex()-1);
    }
  }
  
  public void cleared(){}
  public void nameChanged(String name){}
  /////////////////////////////////////////////////////////////////
  /////////////// Operation On the Selection //////////////////////
  
  public void deleteAll()
  {
    ((FtsBpfObject)itsModel).requestPointsRemove(getMinSelectionIndex(), size());
    
    deselectAll();
  }
  
  public void selectNext()
  {
    if(itsModel.length()>0)
    {
      if(size()==0)
		    select(itsModel.getPointAt(0));	
      else
		    {
        int last = getMaxSelectionIndex();
        if(last < itsModel.length()-1)
        {
          BpfPoint pt = itsModel.getPointAt(last+1); 
          deselectAll();
          select(pt);
        }
		    }
    }
  }
  
  public void selectPrevious()
  {
    if(itsModel.length()>0)
    {
      if(size()==0)
        select(itsModel.getLastPoint());	
      else
      {
        
        int first = getMinSelectionIndex();
        if(first > 0)
	      {
          BpfPoint pt = itsModel.getPointAt(first-1); 
          deselectAll();
          select(pt);
	      }
      }
    }
  }
  
  public void moveSelection(int deltaX, int deltaY, BpfGraphicContext bgc)
  {
    BpfAdapter a = bgc.getAdapter();
    FtsBpfObject ftsObj = (FtsBpfObject)itsModel;
    //clip deltaY///////////////////
    if(deltaY > 0)
    {
      int minY = a.getY(getMinValueInSelection());
      int hMin = a.getY(ftsObj.getMinimumValue());
      if(minY + deltaY > hMin)
        deltaY = hMin - minY;
    }
    else
    {
      int maxY = a.getY(getMaxValueInSelection());
      int hMax = a.getY(ftsObj.getMaximumValue());
      if(maxY + deltaY < hMax)
        deltaY = hMax - maxY;
    }
    
    //clip deltaX///////////////////
    BpfPoint last, first;
    int firstIndex, lastIndex;
    first =  getFirstInSelection();
    firstIndex = getMinSelectionIndex();
    if(size()==1)
    {
      last = first;
      lastIndex = firstIndex;
    }
    else
    {
      last =  getLastInSelection();
      lastIndex = getMaxSelectionIndex();
    }
    int lastX =  a.getX(last);
    int firstX = a.getX(first);
    
    BpfPoint next = ftsObj.getNextPoint(last);
    BpfPoint prev = ftsObj.getPreviousPoint(first);
    int nextX = -1;
    int prevX = -1;
    
    if(next!=null)
      nextX = a.getX(next);
    else
      nextX = a.getX(bgc.getMaximumTime()) - BpfAdapter.DX;
    
    if(prev!=null)
      prevX = a.getX(prev);
    else
      prevX = a.getX(0);
    
    if(lastX + deltaX > nextX) 
      deltaX = nextX-lastX;
    else if(firstX+deltaX < prevX)
      deltaX = prevX-firstX;
    
    // starts a serie of undoable moves
    ((UndoableData) itsModel).beginUpdate();
    
    int i = 0;
    int selSize = size();
    float[] times = new float[selSize];
    float[] values = new float[selSize];
    BpfPoint aPoint;
    for (Enumeration e = getSelected(); e.hasMoreElements();)
    {
      aPoint = (BpfPoint) e.nextElement();
      times[i] = a.getInvX(a.getX(aPoint) + deltaX);
      values[i++] = a.getInvY(a.getY(aPoint)+deltaY);
    }
    
    ftsObj.requestSetPoints(firstIndex, times, values);
  }
  
  public void alignTop()
  {
    float max = getMaxValueInSelection();
    BpfPoint aPoint;
    int i = 0;
    int selSize = size();
    float[] times = new float[selSize];
    float[] values = new float[selSize];
    int firstIndex = getMinSelectionIndex();
    
    ((UndoableData) itsModel).beginUpdate(); 
    
    for (Enumeration e = getSelected(); e.hasMoreElements();)
    {
      aPoint = (BpfPoint) e.nextElement();
      times[i] = aPoint.getTime();
      values[i++] = max;
    }
    ((FtsBpfObject)itsModel).requestSetPoints(firstIndex, times, values);
  }
  public void alignBottom()
  {
    float min = getMinValueInSelection();
    BpfPoint aPoint;
    int i = 0;
    int selSize = size();
    float[] times = new float[selSize];
    float[] values = new float[selSize];
    int firstIndex = getMinSelectionIndex();
    
    ((UndoableData) itsModel).beginUpdate(); 
    
    for (Enumeration e = getSelected(); e.hasMoreElements();)
    {
      aPoint = (BpfPoint) e.nextElement();
      times[i] = aPoint.getTime();
      values[i++] = min;
    }
    ((FtsBpfObject)itsModel).requestSetPoints(firstIndex, times, values);
  }
  public void alignLeft()
  {
    BpfPoint aPoint;
    int i = 0;
    int selSize = size();
    float[] times = new float[selSize];
    float[] values = new float[selSize];
    int firstIndex = getMinSelectionIndex();
    float left = getFirstInSelection().getTime();	
    
    ((UndoableData) itsModel).beginUpdate(); 
    
    for (Enumeration e = getSelected(); e.hasMoreElements();)
    {
      aPoint = (BpfPoint) e.nextElement();
      times[i] = left;
      values[i++] = aPoint.getValue();
    }
    ((FtsBpfObject)itsModel).requestSetPoints(firstIndex, times, values);
  }
  public void alignRight()
  {
    BpfPoint aPoint;
    int i = 0;
    int selSize = size();
    float[] times = new float[selSize];
    float[] values = new float[selSize];
    int firstIndex = getMinSelectionIndex();
    float right = getLastInSelection().getTime();	
    
    ((UndoableData) itsModel).beginUpdate(); 
    
    for (Enumeration e = getSelected(); e.hasMoreElements();)
    {
      aPoint = (BpfPoint) e.nextElement();
      times[i] = right;
      values[i++] = aPoint.getValue();
    }
    ((FtsBpfObject)itsModel).requestSetPoints(firstIndex, times, values);
  }
}














