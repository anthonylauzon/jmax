
package ircam.jmax.editors.explode;

import java.util.*;
import java.awt.datatransfer.*;
import ircam.jmax.toolkit.*;

/**
 * A selection of explode events. Use the static constructor to create the
 * (unique) explode selection of the system.
 */ 
public class ExplodeSelection extends AbstractSelection implements ExplodeDataListener{

  /**
   * constructor
   */
  private ExplodeSelection(ExplodeDataModel theModel) 
  {
    super();
    itsModel = theModel;
    addFlavor(ExplodeDataFlavor.getInstance());
  }

  /**
   * selects all the objects of the data model
   */
  public void selectAll() 
  {
    select(itsModel.getEvents());
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

  /** Transferable interface */
  public Object getTransferData(DataFlavor flavor) 
  {
    return itsCopy; 
  }

  /**
   * Function used by clipboard operations.
   */ 
  ExplodeSelection getACopy()
  {
    if (itsCopy == null) itsCopy = new ExplodeSelection(itsModel);
    else itsCopy.deselectAll();
    
    ScrEvent aEvent;

    for (Enumeration e = getSelected(); e.hasMoreElements();)
      {
	aEvent = (ScrEvent) e.nextElement();
	
	itsCopy.select(new ScrEvent(aEvent.getDataModel(),
				    aEvent.getTime(),
				    aEvent.getPitch(),
				    aEvent.getVelocity(),
				    aEvent.getDuration(),
				    aEvent.getChannel()));
      }

    return itsCopy;
  }

  //--- Fields
  private static ExplodeSelection itsSelection; 
  private static ExplodeSelection itsCopy; // for clipboard use
  ExplodeDataModel itsModel;

}


