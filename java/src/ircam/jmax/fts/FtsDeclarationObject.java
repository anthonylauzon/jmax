package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsDeclarationObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject declaration.
   * Note that for declarations, we assume the description include the class name
   */

  FtsDeclarationObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/

  /** Save the object to a TCL stream. */

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "declare ..."

    writer.print("declare {" + description + "}");

    savePropertiesAsTcl(writer);
  }
}






