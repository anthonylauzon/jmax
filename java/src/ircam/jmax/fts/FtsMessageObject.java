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

public class FtsMessageObject extends FtsObject
{
  /**
   * MessboxMessageHandler interpret the dedicated messages 
   * coming from FTS to the Messbox object
   */

  class MessboxMessageHandler implements FtsMessageHandler
  {
    public void handleMessage(FtsMessage msg)
    {
      FtsMessageObject.this.localPut("value", FtsObject.makeDescription(2, msg));
    }
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */


  public FtsMessageObject(FtsContainerObject parent, String description, int objId)
  {
    super(parent, "messbox", description, objId);

    ninlets = 1;
    noutlets = 1;

    installMessageHandler(new MessboxMessageHandler());
  }

  /**
   * redefine the message
   */

  public void setMessage(String message)
  {
    description = message;

    Fts.getServer().sendSetMessage(this, description);
    setDirty();
  }
}






