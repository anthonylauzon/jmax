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

  /**
   * Create a FtsObject object;
   */

  public FtsMessageObject(FtsContainerObject parent, String description, int objId)
  {
    super(parent, "messbox", description, objId);

    ninlets = 1;
    noutlets = 1;
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

  /* Over write the handle message to handle message box changes */

  public void handleMessage(FtsMessage msg)
  {
    description = FtsParse.unparseObjectDescription(2, msg);

    FtsMessageObject.this.localPut("value", description);
  }
}






