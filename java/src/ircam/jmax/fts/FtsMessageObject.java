//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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

    public FtsMessageObject(Fts fts, FtsObject parent/*, int objId*/, String description)
  {
      super(fts, parent/*, objId*/, "messbox", description);

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

  public void handleMessage(String selector, int nArgs, FtsAtom args[])
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    this.message = FtsParse.unparseArguments(nArgs, args);
    setDirty();

    if (listener instanceof FtsMessageListener)
      ((FtsMessageListener) listener).messageChanged(message);
  }
}






