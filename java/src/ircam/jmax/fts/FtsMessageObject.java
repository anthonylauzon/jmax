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
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  String message; // the message content

  /**
   * Create a FtsObject object;
   */

  public FtsMessageObject(FtsObject parent, String description, int objId)
  {
    super(parent, "messbox", null, description, objId);

    ninlets = 1;
    noutlets = 1;

    message = description;
  }

  public void setMessage(String message)
  {
    this.message = message;
    Fts.getServer().sendSetMessage(this, message);
    setDirty();
  }

  public String getMessage()
  {
    return message;
  }
       
  /* Over write the handle message to handle message box changes */

  void handleMessage(FtsMessage msg)
  {
    setMessage(FtsParse.unparseObjectDescription(msg));

    if (listener instanceof FtsMessageListener)
      ((FtsMessageListener) listener).messageChanged(message);
  }
}






