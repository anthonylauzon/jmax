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

public class FtsStandardObject extends FtsObject
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

  FtsStandardObject(FtsContainerObject parent, String className, Vector args)
  {
    super(parent, className, args);

    MaxApplication.getFtsServer().newObject(parent, this, args);

    if (parent.isOpen())
      {
	updated = true;
	getProperty("ninlets");
	getProperty("noutlets");

	MaxApplication.getFtsServer().syncToFts();
      }
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/


  /**
   * Set the arguments
   * This may require an object redefinition
   * on the FTS side; this may then imply some comunication with FTS.
   * Actually this should be possible only for message and comment
   */

  public void setArguments(Vector args)
  {
    this.args = args;

    // Use fts class name, not user class name

    MaxApplication.getFtsServer().redefineObject(this, ftsClassName, args);

    if (parent.isOpen() && updated)
      {
	getProperty("ninlets");
	getProperty("noutlets");
	MaxApplication.getFtsServer().syncToFts();
      }
  }

  void saveAsTcl(FtsSaveStream stream)
  {
    // Save as "object ..."

    stream.print("object $objs(" + parent.idx + ") ");

    saveArgsAsTcl(stream);

    stream.print(" ");

    if (graphicDescr != null)
      graphicDescr.saveAsTcl(stream);
  }
}






