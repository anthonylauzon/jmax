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

public class FtsAbstractionObject  extends FtsContainerObject
{
  static boolean  expandAbstractions = true;

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  // For an abstraction loaded from FTS, just behave as a standard object.

  FtsAbstractionObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);

    this.className = className;
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/


  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "object ..."

    writer.print("object {" + description + "}");

    savePropertiesAsTcl(writer);
  }
}






