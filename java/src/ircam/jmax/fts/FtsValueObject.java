package ircam.jmax.fts;


import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsValueObject extends FtsStandardObject
{
  Object value;

  /* code to set generic properties meta-properties */

  static
  {
    FtsPropertyDescriptor.setPersistent("initValue", true);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */


  FtsValueObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);
  }

  /* if you want value to become a "java beans property"
     this code is the beginning */

  /*
  protected void builtinPropertyNames(Vector names)
  {
    names.addElement("value");
    super.builtinPropertyNames(names);
  }

  protected boolean builtinPut(String name, Object value)
  {
    if (name.equals("value"))
      {
	this.value = value;
	return true;
      }
    else
      return super.builtinPut(name, value);
  }

  protected Object builtinGet(String name)
  {
    if (name.equals("value"))
      return value;
    else
      return super.builtinGet(name);
  }
  */
}







