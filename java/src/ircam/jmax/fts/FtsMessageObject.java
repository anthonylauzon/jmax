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

public class FtsMessageObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */

  public FtsMessageObject(FtsContainerObject parent, String description)
  {
    super(parent, "message", description);

    MaxApplication.getFtsServer().newObject(parent, this, "message", description);

    ninlets = 1;
    noutlets = 1;
  }

  /**
   * redefine the message
   */

  public void setMessage(String message)
  {
    description = message;

    MaxApplication.getFtsServer().redefineMessageObject(this, description);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/

  public void saveAsTcl(PrintWriter writer)
  {
    // Save as "declare ..."

    writer.print("object {message {" + description + "}}");

    savePropertiesAsTcl(writer);

  }
}






