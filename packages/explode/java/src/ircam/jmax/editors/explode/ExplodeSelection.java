
package ircam.jmax.editors.explode;

import java.util.*;

/**
 * A selection of explode events. Use the static constructor to create the
 * (unique) selection of the system.
 */ 
public class ExplodeSelection implements SelectionHandler, ExplodeDataListener {

  /**
   * constructor
   */
  private ExplodeSelection(ExplodeDataModel theModel) 
  {
    itsModel = theModel;
    selected = new Hashtable();
    listeners = new Vector();
  }

  /**
   * select the given object 
   */
  public void select(Object obj) 
  {
    selected.put(obj, obj);
    notifyListeners();
  }

  /**
   * remove the object from the selection
   */
  public void deSelect(Object obj) 
  {
    selected.remove(obj);
    notifyListeners();
  }


  /**
   * returns true if the object is currently selected
   */
  public boolean isInSelection(Object obj) 
  {
    return selected.containsKey(obj);
  }


  /**
   * returns an enumeration of all the selected objects
   */
  public Enumeration getSelected() 
  {
    return selected.elements();
  }


  /**
   * deselects all the objects
   */
  public void deselectAll() 
  {
    selected.clear();
    notifyListeners();
  }


  /**
   * selects all the objects of the data model
   */
  public void selectAll() 
  {
    for (Enumeration e = itsModel.getEvents(); e.hasMoreElements();) 
      {
	select(e.nextElement());
      }
    notifyListeners();
  }

  /**
   * static constructor
   */
  static public ExplodeSelection createSelection(ExplodeDataModel ep) 
  {
    if (itsSelection == null) 
      {
	itsSelection = new ExplodeSelection(ep);
	ep.addListener(itsSelection);
      }

    return itsSelection;
  }

  /**
   * returns the (unique) selection
   */
  public static ExplodeSelection getSelection()
  {
    return itsSelection;
  }

  /**
   * require to be notified when data change
   */
  public static void addListener(ExplodeDataListener theListener) 
  {
    listeners.addElement(theListener);
  }

  /**
   * remove the listener
   */
  public static void removeListener(ExplodeDataListener theListener) 
  {
    listeners.removeElement(theListener);
  }


  /**
   * call back the listener, the selection content changed
   */
  private static void notifyListeners()
  {
    SelectionListener sl;

    for (Enumeration e = listeners.elements(); e.hasMoreElements();)
      {
	sl = (SelectionListener) e.nextElement();
	sl.selectionChanged();
      }
  }


  /** ExplodeDataListener interface*/

  public void objectChanged(Object spec) 
  {
  }

  public void objectAdded(Object spec) 
  {
  }

  /** ExplodeDataListener interface */
  public void objectDeleted(Object whichObject) 
  {
    if (isInSelection(whichObject))
      deSelect(whichObject);
  }

  //--- Fields
  private static ExplodeSelection itsSelection; 
  ExplodeDataModel itsModel;
  static Hashtable selected;

  private static Vector listeners;
}


