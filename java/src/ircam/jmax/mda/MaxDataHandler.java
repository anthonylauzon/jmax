package ircam.jmax.mda; 

import java.util.*;

/** A Data Handler is an object able to load a Max Data instance
 *  from a data source; the class also handle the data base of all
 *  the existing MaxDataHandler, and provide a static function
 *  to find the data handler can reconize an address, and another
 *  to directly load the instance.
 */

abstract public class MaxDataHandler
{
  /** The data handler able to load/save instances of the type */

  static Vector allHandlers = new Vector();

  /** Static method to find a Data Handler for a given Data Source;
   *  Use canSaveTo because in general we cannot load from a data source
   * that correspond to an empty file ...
   */

  public static MaxDataHandler findDataHandlerFor(MaxDataSource source)
  {
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDataHandler dataHandler;

	dataHandler = (MaxDataHandler) allHandlers.elementAt(i);

	if (dataHandler.canSaveTo(source))
	  return dataHandler;
      }

    return null;
  }


  /** Load an instance from a given data source) */

  public static MaxData loadDataInstance(MaxDataSource source) throws MaxDataException
  {
    MaxData newInstance;
    
    for (int i = 0; i < allHandlers.size() ; i++)
      {
	MaxDataHandler dataHandler;

	dataHandler = (MaxDataHandler) allHandlers.elementAt(i);

	if (dataHandler.canLoadFrom(source))
	  {
	    newInstance = dataHandler.loadInstance(source);

	    newInstance.setDataHandler(dataHandler);
	    newInstance.setDataSource(source);

	    // here, raise the new instance event  ??

	    return newInstance;
	  }
      }

    throw new MaxDataException("Internal error: cannot load from " + source);
  }


  /**  handle registration in the global
   * table; note that Data Handlers cannot be deinstalled, once
   * created; handler have to be installed after the creation,
   * before using them.
   */

  static public void installDataHandler(MaxDataHandler handler)
  {
    allHandlers.addElement(handler);
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







