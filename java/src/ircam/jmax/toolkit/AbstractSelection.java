package ircam.jmax.toolkit;

import java.util.*;

import ircam.jmax.utils.*;

/**
 * An abstract base class implementing the selection interface,
 * taking care of the listener's mechanism; actually, only the 
 * selectAll() method needs to be implemented.
 * @see SelectionHandler
 */
public abstract class AbstractSelection implements SelectionHandler {
  

  /**
   * constructor
   */
  public AbstractSelection() 
  {
    selected = new MaxVector();
    listeners = new MaxVector();
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


  //--- Fields

  protected MaxVector selected;
  protected MaxVector listeners;

  public static final int OBJECT_SELECTED = 1;
  public static final int GROUP_SELECTED = 2;
  public static final int OBJECT_DESELECTED = 4;
  public static final int GROUP_DESELECTED = 8;
  public static final int SELECTION_CHANGED = 16;

  public static final int ALL_EVENTS = OBJECT_SELECTED | GROUP_SELECTED | OBJECT_DESELECTED | GROUP_DESELECTED | SELECTION_CHANGED;
}
