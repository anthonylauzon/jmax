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

package ircam.jmax.editors.explode;

import java.util.*;
import javax.swing.*;
import java.awt.datatransfer.*;
import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import java.lang.reflect.*;

/**
 * The Explode selection. This class is implemented on the base of the
 * Swing's ListSelectionModel, so that it can be used unchanged in
 * the JTable representation of the score.
 * Even if every Explode editor have its own selection, just one of them
 * is active at a time. The active selection is set via the setCurrent method.
 * An ExplodeSelection has an owner (a SelectionOwner implementor): this 
 * Owner will be called back when the selection become active or disactive.
 * The PartitionEventRenderer objects use the ownership information to draw
 * the selected event differently (selected-active / selected-non active).
 * @see ExplodeTablePanel */
public class ExplodeSelection extends DefaultListSelectionModel implements ExplodeDataListener, Transferable, Cloneable{

  
  //--- Fields
  private static ExplodeSelection current;
  private static MaxVector itsCopy = new MaxVector();

  public static DataFlavor flavors[];

  private MaxVector temp = new MaxVector();

  private SelectionOwner itsOwner;  
  protected  ExplodeDataModel itsModel;

  // Implementation notes: 
  // The ListSelectionModel, and then the ExplodeSelection, 
  // are based on indexes, while the identity
  // of an explode event is only partially related to its index 
  // (infact, it can be moved around in the DB).
  // This imposes some extra-care while dealing with moving and deleting 
  // operation.
  // See also the notes in the objectMoved call.
  ExplodeSelection(ExplodeDataModel model) 
  {
    setModel(model);
    setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION) ;
    if (flavors == null)
      flavors = new DataFlavor[1];
    flavors[0] = ExplodeDataFlavor.getInstance();

    // make this selection a listener of its own data model
    model.addListener(this);
  }
   
  ExplodeSelection()
  {
    if (flavors == null)
      flavors = new DataFlavor[1];
    flavors[0] = ExplodeDataFlavor.getInstance();
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
   * Returns the ExplodeDataModel this selection refers to */
  public ExplodeDataModel getModel()
  {
    return itsModel;
  }

  /**
   * Sets the ExplodeDataModel this selection refers to */
  public void setModel(ExplodeDataModel m)
  {
    itsModel = m;
  }

  /**
   * Sets the current active selection. This will send a 
   * selectionActivated message to that selection's owner, and a 
   * selectionDisactivated to the old selection's owner */
  public static void setCurrent(ExplodeSelection s)
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
    int index = itsModel.indexOf((ScrEvent) obj);

    if (!isSelectedIndex(index)) 
      addSelectionInterval(index, index);
  }
  
  /** Select the given enumeration of objects.
   * When possible, use this method instead of
   * selecting single objects. 
   */
  public void select(Enumeration e)
  {
    while (e.hasMoreElements())
      {
	select(e.nextElement());
      }
  }

  /** remove the given object from the selection
   */
  public void deSelect(Object obj)
  {
    int index = itsModel.indexOf((ScrEvent) obj);
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
    int index = itsModel.indexOf((ScrEvent) obj);
    
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

    clearSelection();
  }


  /**
   * returns the (unique) selection
   */
  public static ExplodeSelection getCurrent()
  {
    return current;
  }  

  /** ExplodeDataListener interface*/

  public void objectChanged(Object spec) 
  {
  }

  public void objectAdded(Object spec, int index) 
  {
    int i;

    if (index > getMaxSelectionIndex()) return;
    for (i = index+1; i >  getMinSelectionIndex(); i--)
      {
	if (isSelectedIndex(i-1))
	  addSelectionInterval(i, i);
	else removeSelectionInterval(i, i);
      }
  
  }

  /**
   * ExplodeDataListener interface: keep the index-based selection consistent. */
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

  /** ExplodeDataListener interface */
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
    int dim = Array.getLength(flavors);
    DataFlavor temp[] = new DataFlavor[dim+1];
    for (int i = 0; i < dim; i++){
      temp[i] = flavors[i];
    }
    temp[dim+1]=flavor;
    flavors = temp;
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
  void prepareACopy()
  {
    itsCopy.removeAllElements();

    ScrEvent s;
    try {

      for (Enumeration e= new ReadOnlyEnum(); e.hasMoreElements();)
	{
	  s = (ScrEvent) e.nextElement();
	  itsCopy.addElement(s.duplicate());
	}

    } catch (Exception ex) {System.err.println("error while cloning events");}
  }

  static void discardTheCopy()
  {
    itsCopy.removeAllElements();
  }
}











