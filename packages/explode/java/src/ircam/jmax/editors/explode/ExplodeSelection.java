
package ircam.jmax.editors.explode;

import java.util.*;

/**
 * A selection of explode events.
 */ 
public class ExplodeSelection implements SelectionHandler {

  /**
   * constructor
   */
  public ExplodeSelection(ExplodeDataModel theModel) 
  {
    itsModel = theModel;
    selected = new Hashtable();
  }

  /**
   * select the object 
   */
  public void select(Object obj) 
  {
    selected.put(obj, obj);
  }

  /**
   * remove the object from the selection
   */
  public void deSelect(Object obj) 
  {
    selected.remove(obj);
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
  }

  //--- Fields
  ExplodeDataModel itsModel;
  Hashtable selected;
}


