//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
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
 * Class implementing the proxy of a message box.
 * 
 * If the listener of this object is an instance
 * of FtsMessageListener, fire it when the we got a new message content
 * from the server.
 */

public class FtsMessageObject extends FtsIntValueObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  String message; // the message content

  public FtsMessageObject(Fts fts, FtsObject parent, String description, int objId)
  {
    super(fts, parent, "messbox", description, objId);

    ninlets = 1;
    noutlets = 1;

    message = description;
  }

  /** Set the message content. Tell the server, too */

  public void setMessage(String message)
  {
    this.message = message;
    fts.getServer().sendSetMessage(this, message);
    setDirty();
  }

  /** Get the message content. */

  public String getMessage()
  {
    return message;
  }
       
  /** Over write the handle message to handle message box changes. */

  void handleMessage(FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    this.message = FtsParse.unparseObjectDescription(stream);
    setDirty();

    if (listener instanceof FtsMessageListener)
      ((FtsMessageListener) listener).messageChanged(message);
  }
}






