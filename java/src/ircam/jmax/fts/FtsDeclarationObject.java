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
   * Create a FtsObject object.
   * Note that there is a small set of FTS primitive classes that are
   * known to the application layer, and require special handling, in 
   * particular "patcher", "inlet" and "outlet".
   */

  FtsDeclarationObject(FtsContainerObject parent, String className, Vector args)
  {
    super(parent, className, args);

    MaxApplication.getFtsServer().newObject(parent, this, args);

  }

  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/



  /**
   * Set the arguments.
   * You cannot change arguments in a declaration.
   */

  public void setArguments(Vector args)
  {
    return;
  }

  /** Save the object to a TCL stream. */

  void saveAsTcl(FtsSaveStream stream)
  {
    // Save as "declare ..."

    stream.print("declare $objs(" + parent.idx + ") ");

    saveArgsAsTcl(stream);

    stream.print(" ");

    if (graphicDescr != null)
      graphicDescr.saveAsTcl(stream);
  }

}






