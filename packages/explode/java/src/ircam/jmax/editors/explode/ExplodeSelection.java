
package ircam.jmax.editors.explode;

import java.util.*;
import ircam.jmax.toolkit.*;

/**
 * A selection of explode events. Use the static constructor to create the
 * (unique) explode selection of the system.
 */ 
public class ExplodeSelection extends AbstractSelection implements ExplodeDataListener {

  /**
   * constructor
   */
  private ExplodeSelection(ExplodeDataModel theModel) 
  {
    super();
    itsModel = theModel;
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

}


