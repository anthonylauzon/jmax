package ircam.jmax.toolkit;

import java.util.*;

import ircam.jmax.utils.*;

/**
 * An abstract base class implementing the selection interface.
 * It takes care of the listener's mechanism. Actually, only the 
 * selectAll() method needs to be implemented.
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
    notifyListeners();
  }

  /**
   * remove the object from the selection
   */
  public void deSelect(Object obj) 
  {
    selected.removeElement(obj);
    notifyListeners();
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


  public int size()
  {
    return selected.size();
  }

  /**
   * deselects all the objects
   */
  public void deselectAll() 
  {
    selected.removeAllElements();
    notifyListeners();
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
  protected void notifyListeners()
  {
    SelectionListener sl;

    for (Enumeration e = listeners.elements(); e.hasMoreElements();)
      {
	sl = (SelectionListener) e.nextElement();
	sl.selectionChanged();
      }
  }


  //--- Fields

  protected MaxVector selected;
  protected MaxVector listeners;
}
