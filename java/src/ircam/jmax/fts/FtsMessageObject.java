//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
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

  void handleMessage(FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    this.message = FtsParse.unparseObjectDescription(stream);
    setDirty();

    if (listener instanceof FtsMessageListener)
      ((FtsMessageListener) listener).messageChanged(message);
  }
}






