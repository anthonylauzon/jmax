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

package ircam.jmax.toolkit;

import java.util.*;
import java.awt.datatransfer.*;

import ircam.jmax.utils.*;

/**
 * An abstract base class implementing the selection interface,
 * taking care of the listener's mechanism; actually, only the 
 * selectAll() method needs to be implemented.
 * An abstract selection implements the Transferable interface, in
 * order to provide a support for clipboard operations based on selections.
 * Add the flavours this selection can support via the addFlavour method,
 * And redefine the getTransferData method in order to return the
 * Transferable suited for your kind of object
 * @see SelectionHandler
 */
public abstract class AbstractSelection implements SelectionHandler, Transferable {
  

  /**
   * constructor
   */
  public AbstractSelection() 
  {
    selected = new MaxVector();
    listeners = new MaxVector();
    dataFlavors = new MaxVector();
  }

  /**
   * select the given object 
   */
  public void select(Object obj) 
  {
    selected.addElement(obj);
    notifyListeners(OBJECT_SELECTED);
  }

  /**
   * select the given enumeration of objects 
   */
  public void select(Enumeration e) 
  {
    int count = 0;
    while(e.hasMoreElements())
      {
	selected.addElement(e.nextElement());
	count++;
      }

    if (count == 1) notifyListeners(OBJECT_SELECTED);
    else if (count > 1) notifyListeners(GROUP_SELECTED);
  }

  /**
   * remove the object from the selection (deselect the given object)
   */
  public void deSelect(Object obj) 
  {
    selected.removeElement(obj);
    notifyListeners(OBJECT_DESELECTED);
  }

  /**
   * remove the Enumeration of object from the selection (deselect the given
   * enumeration of objects)
   */
  public void deSelect(Enumeration e) 
  {
    int count = 0;
    while (e.hasMoreElements())
      {
	selected.removeElement(e.nextElement());
	count++;
      }

    if (count ==1)
      notifyListeners(OBJECT_DESELECTED);
    else if (count > 1) notifyListeners(GROUP_DESELECTED);
  }


  /**
   * returns true if the object is currently selected
   */
  public boolean isInSelection(Object obj) 
  {
    return selected.contains(obj);
  }


  /**
   * returns an enumeration of all the selected objects
   */
  public Enumeration getSelected() 
  {
    return selected.elements();
  }

  /**
   * returns the lenght of the selection (how many selected elements) */
  public int size()
  {
    return selected.size();
  }

  /**
   * deselects all the objects currently selected
   */
  public void deselectAll() 
  {
    selected.removeAllElements();
    notifyListeners(GROUP_DESELECTED);
  }


  /**
   * selects all the objects of the data model
   */
  public abstract void selectAll();


  /**
   * require to be notified when data change
   */
  public void addSelectionListener(SelectionListener theListener) 
  {
    listeners.addElement(theListener);
  }

  /**
   * remove the listener
   */
  public void removeSelectionListener(SelectionListener theListener) 
  {
    listeners.removeElement(theListener);
  }


  /**
   * call back the listener, the selection content changed
   */
  protected void notifyListeners(int type)
  {
    SelectionListener sl;

    for (Enumeration e = listeners.elements(); e.hasMoreElements();)
      {
	sl = (SelectionListener) e.nextElement();
	
	switch (type) {
	case OBJECT_SELECTED: 
	  sl.objectSelected();
	  break;
	case GROUP_SELECTED: 
	  sl.groupSelected();
	  break;
	case OBJECT_DESELECTED: 
	  sl.objectDeselected();
	  break;
	case GROUP_DESELECTED: 
	  sl.groupDeselected();
	  break;
	}
      }
  }

  /** utility function */
  protected void addFlavor(DataFlavor flavor)
  {
    dataFlavors.addElement(flavor);
  }

  /** Transferable interface */
  public boolean isDataFlavorSupported(DataFlavor flavor)
  {
    for (int i = 0; i < dataFlavors.size(); i++)
      {
	if (flavor.equals(dataFlavors.getObjectArray()[i]))
	  return true;
      }
    return false;
  }

  /** Transferable interface */
  public DataFlavor[] getTransferDataFlavors()
  {
    return (DataFlavor[]) dataFlavors.getObjectArray();
  }

  /** Transferable interface */
  public Object getTransferData(DataFlavor flavor) 
  {
    return null;
  }

  //--- Fields

  public MaxVector selected;
  protected MaxVector listeners;
  protected MaxVector dataFlavors;

  public static final int OBJECT_SELECTED = 1;
  public static final int GROUP_SELECTED = 2;
  public static final int OBJECT_DESELECTED = 4;
  public static final int GROUP_DESELECTED = 8;
  public static final int SELECTION_CHANGED = 16;

  public static final int ALL_EVENTS = OBJECT_SELECTED | GROUP_SELECTED | OBJECT_DESELECTED | GROUP_DESELECTED | SELECTION_CHANGED;
}
