package ircam.jmax.mda; 

import java.util.*;

/** A Data Handler is an object able to load a Max Data instance
 *  from a data source; the class also handle the data base of all
 *  the existing MaxDataHandler, and provide a static function
 *  to find the data handler can reconize an address, and another
 *  to directly load the instance.
 */

abstract class MaxDataHandler
{
  /** The data handler able to load/save instances of the type */

  static Vector allHandlers;

  /** Static method to find a Data Handler for a given Data Source */

  public static MaxDataHandler findDataHandlerFor(MaxDataSource source)
  {
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDataHandler dataHandler;

	dataHandler = (MaxDataHandler) allHandlers.elementAt(i);

	if (dataHandler.canLoadFrom(source))
	  return dataHandler;
      }

    return null;
  }


  /** Load an instance from a given data source)

  public static MaxData loadDataInstance(MaxDataSource source)
  {
    MaxData newInstance;
    
    for (i = 0; i < allHandlers.size() ; i++)
      {
	MaxDataHandler dataHandler;

	dataHandler = (MaxDataHandler) elementAt(i);

	if (dataHandler.canLoadFrom(dataSource))
	  {
	    newInstance = dataHandler.loadInstance(dataSource);

	    newInstance.setDataHandler(dataHandler);
	    newInstance.setDataSource(dataSource);

	    // here, raise the new instance event  ??

	    return newInstance;
	  }
      }

    throw new MaxDataException("Internal error: cannot load from " + source);
  }


  /**  Constructor: just handle registration in the global
   * table; note that Data Handlers cannot be deinstalled, once
   * created
   */

  MaxDataHandler()
  {
    allHandlers.addElement(this);
  }

  /** Return true if this Data Handler can load
    from the given address; by default return false */

  public boolean canLoadFrom(MaxDataSource source)
  {
    return false;
  }


  /** Return true if this data handler can save 
    to the given address; by default return false */

  public boolean canSaveTo(MaxDataSource source)
  {
    return false;
  }

  /** Load an instance from a data source.
    Do not overwrite this method, use makeInstance instead,
    in the proper subclass.

    If you want to call this, probabily you really want to call
    the static loadDataInstance.
    */

  abstract protected MaxData loadInstance(MaxDataSource source) throws MaxDataException;

  abstract public void saveInstance(MaxData instance) throws MaxDataException;
       
}



