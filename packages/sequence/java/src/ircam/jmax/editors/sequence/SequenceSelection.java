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

package ircam.jmax.editors.sequence;

import ircam.jmax.*;
import ircam.jmax.editors.sequence.track.*;
import java.util.*;
import javax.swing.*;
import java.awt.datatransfer.*;
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
public class SequenceSelection extends DefaultListSelectionModel implements TrackDataListener, Transferable, Cloneable{

  
  //--- Fields
  private static SequenceSelection current;
    private static MaxVector itsCopy = new MaxVector();

  public DataFlavor flavors[];

  private MaxVector temp = new MaxVector();

  private SelectionOwner itsOwner;  
  protected  TrackDataModel itsModel;
  protected TrackEvent lastSelectedEvent = null;

  // Implementation notes: 
  // The ListSelectionModel, and then the SequenceSelection, 
  // are based on indexes, while the identity
  // of an sequence event is only partially related to its index 
  // (infact, it can be moved around in the DB).
  // This imposes some extra-care while dealing with moving and deleting 
  // operation.
  // See also the notes in the objectMoved call.
  public SequenceSelection(TrackDataModel model) 
  {
    itsModel = model;
    setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION) ;
    initDataFlavors();

    // make this selection a listener of its own data model
    model.addListener(this);
  }
   
  public SequenceSelection()
  {
    if (flavors == null)
      flavors = new DataFlavor[1];
    flavors[0] = SequenceDataFlavor.getInstance();
  }

  void initDataFlavors()
    {
	if (flavors == null)
	    flavors = new DataFlavor[1];
	flavors[0] = SequenceDataFlavor.getInstance();
	
	DataFlavor[] trackFlavors = ((FtsTrackObject)itsModel).getDataFlavors();
	for(int i=0; i<trackFlavors.length;i++)
	    addFlavor(trackFlavors[i]);
    }

  public void addSelectionInterval(int index1, int index2)
  {
    super.addSelectionInterval(index1, index2);
  }

  /**
   * Ownership handling
   */
  public void setOwner(SelectionOwner so)
  {
    itsOwner = so;
  }

  /**
   * Ownership handling
   */
  public SelectionOwner getOwner()
  {
    return itsOwner;
  }

  /**
   * Returns the SequenceDataModel this selection refers to */
  public TrackDataModel getModel()
  {
    return itsModel;
  }

  /**
   * Sets the TrackDataModel this selection refers to
  public void setModel(TrackDataModel m)
  {
      if (itsModel != m)
	  deselectAll();
      if (itsModel != null)
	  itsModel.removeListener(this);
      itsModel = m;
      if (m != null) 
	  m.addListener(this);
	  }*/

  /**
   * Sets the current active selection. This will send a 
   * selectionActivated message to that selection's owner, and a 
   * selectionDisactivated to the old selection's owner */
  public static void setCurrent(SequenceSelection s)
  {
    if (current != null && current.itsOwner != null)
      current.itsOwner.selectionDisactivated();

    current = s;

    if (s != null && s.itsOwner != null)
      s.itsOwner.selectionActivated();

  }

  /** select the given object 
   */
  public void select(Object obj)
  {
    int index = itsModel.indexOf((TrackEvent) obj);

    if (!isSelectedIndex(index)) 
	{
	    lastSelectedEvent = (TrackEvent)obj;
	    addSelectionInterval(index, index);
	}
  }
  
  /** Select the given enumeration of objects.
   * When possible, use this method instead of
   * selecting single objects. 
   */
  public void select(Enumeration e)
  {
      TrackEvent event;
      setValueIsAdjusting(true);
      while(e.hasMoreElements())
	  {
	      event = (TrackEvent)e.nextElement();
	      select(event);
	  }
      setValueIsAdjusting(false);
  }

  /** remove the given object from the selection
   */
  public void deSelect(Object obj)
  {
    int index = itsModel.indexOf((TrackEvent) obj);
    if(obj == lastSelectedEvent) lastSelectedEvent = null;
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


  /** returns true if the object is currently selected
   */
  public boolean isInSelection(Object obj)
  {
    int index = itsModel.indexOf((TrackEvent) obj);
    
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
      if (isSelectedIndex(i)) temp.addElement(itsModel.getEventAt(i));
    
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
  }

  public TrackEvent getLastSelectedEvent()
    {
	return lastSelectedEvent;
    }
  public void setLastSelectedEvent(TrackEvent last)
    {
	lastSelectedEvent = last;
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
      lastSelectedEvent = null;
      clearSelection();
  }


  /**
   * returns the (unique) selection
   */
  public static SequenceSelection getCurrent()
  {
    return current;
  }  

  /** TrackDataListener interface*/

  public void objectChanged(Object spec, String propName, Object propValue) 
  {
  }

  public void objectAdded(Object spec, int index) 
  {
      /*int i;

	if (index > getMaxSelectionIndex()) return;
	for (i = index+1; i >  getMinSelectionIndex(); i--)
	{
	if (isSelectedIndex(i-1))
	addSelectionInterval(i, i);
	else removeSelectionInterval(i, i);
	}*/
      //deselectAll();
      select(spec);    
  }
  public void objectsAdded(int maxTime) {}

  public void lastObjectMoved(Object o, int oldIndex, int newIndex){}
  /**
   * TrackDataListener interface: keep the index-based selection consistent. */
  public void objectMoved(Object o, int oldIndex, int newIndex)
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

  /** TrackDataListener interface */
  public void objectDeleted(Object whichObject, int oldIndex) 
  {
    // The implementation is very conservative. It does not make the 
    // assumption that a deleted object was selected... 
    // It could also be implemented with a simple clearSelection() call
    
    for (int i = oldIndex; i<=getMaxSelectionIndex(); i++)
      {
	if (isSelectedIndex(i+1))
	  {
	    addSelectionInterval(i, i);
	  }
	else
	  {
	    removeSelectionInterval(i,i);
	  }
      }
  }

  /** TrackDataListener interface */
  public void trackCleared() 
  {
      deselectAll();
  }
    public void trackNameChanged(String oldName, String newName){}

  /** Transferable interface */
  public Object getTransferData(DataFlavor flavor) 
  {
      return itsCopy.elements(); 
  }

  /** Transferable interface */
  public boolean isDataFlavorSupported(DataFlavor flavor)
  {
    for (int i = 0; i < Array.getLength(flavors); i++)
      {
	if (flavor.equals(flavors[i]))
	  return true;
      }
    return false;
  }

  public DataFlavor[]  getTransferDataFlavors() 
  {
    return flavors;
  }

  /** utility function */
  protected void addFlavor(DataFlavor flavor)
  {
      //int dim = Array.getLength(flavors);
      int dim = flavors.length;
    DataFlavor temp[] = new DataFlavor[dim+1];
    for (int i = 0; i < dim; i++){
      temp[i] = flavors[i];
    }
    //temp[dim+1]=flavor;
    temp[dim]=flavor;
    flavors = temp;
  }

    /////////////////////////////////////////////////////////////////
    /////////////// Operation On the Selection //////////////////////

    public void deleteAll()
    {
	if(size()==itsModel.length())
	    {			
		deselectAll();
		((FtsTrackObject)itsModel).requestClearTrack();			    
	    }
	else
	    {
		MaxVector v = new MaxVector();		    
		for (Enumeration en = getSelected(); en.hasMoreElements();)
		    v.addElement(en.nextElement());

		deselectAll();

		itsModel.removeEvents(v.elements());
		v = null;
	    }
    }

    public void selectNext()
    {
	if(itsModel.length()>0)
	    {
		if(size()==0)
		    select(itsModel.getEventAt(0));	
		else
		    {
			int last = getMaxSelectionIndex();
			if(last < itsModel.length()-1)
			    {
				TrackEvent evt = itsModel.getEventAt(last+1); 
				deselectAll();
				select(evt);
			    }
		    }
	    }
    }

    public void selectPrevious()
    {
	if(itsModel.length()>0)
	    {
		if(size()==0)
		    select(itsModel.getLastEvent());	
		else
		    {
	
			int first = getMinSelectionIndex();
			if(first > 0)
			    {
				TrackEvent evt = itsModel.getEventAt(first-1); 
				deselectAll();
				select(evt);
			    }
		    }
	    }
    }
  /**
   * An usefull (and fast) class to traverse the selection, to be used
   * when the objects are not changed in the loop (read only) */
  class ReadOnlyEnum implements Enumeration
  {

    ReadOnlyEnum()
    {
      current = getMinSelectionIndex();
    }

    public boolean hasMoreElements()
    {
      next = findNext();
      return next != NO_MORE;
    }

    public Object nextElement()
    {
      return itsModel.getEventAt(next);
    }

    int findNext()
    {
      while (current <= getMaxSelectionIndex())
	{
	  if (isSelectedIndex(current))
	    {
	      current++;
	      return current-1;
	    }
	  else current++;
	}
      return NO_MORE;
    }

    //--- Fields
    int current;
    int next;
    final int NO_MORE = -1;
  }

  /**
   * Function used by clipboard operations.
   * This method makes a clone of the objects currently selected.
   */ 
  public void prepareACopy()
  {
    itsCopy.removeAllElements();

    TrackEvent s;
    try {

      for (Enumeration e= new ReadOnlyEnum(); e.hasMoreElements();)
	{
	  s = (TrackEvent) e.nextElement();
	  itsCopy.addElement(s.duplicate());
	}

    } catch (Exception ex) {System.err.println("error while cloning events");}

  }

  static void discardTheCopy()
  {
    itsCopy.removeAllElements();
  }
}











