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

import ircam.jmax.*;
import java.io.*;
import java.util.*;

/**
 * Class implementing the proxy of an Fts connection in the client
 * deals with the connection creation/deletion and saving/loading
 */

public class FtsConnection 
{
  /** to protect against double deletion.
    temporary, waiting for a more event/based editor
    */

  private int id;

  boolean deleted = false; 
  FtsObject from;
  int outlet;

  FtsObject to;
  int inlet;

  /** Create a FTS connection.  */

  FtsConnection(FtsPatcherData data, int id, FtsObject from, int outlet, FtsObject to, int inlet)
  {
    super();
   
    this.id     = id;
    this.from   = from;
    this.outlet = outlet;
    this.to     = to;
    this.inlet  = inlet;

    if (data != null)
      data.addConnection(this);
  }

  void redefine(FtsObject from, int outlet, FtsObject to, int inlet)
  {
    this.from   = from;
    this.outlet = outlet;
    this.to     = to;
    this.inlet  = inlet;
  }

  /**
   * Get the fts connection id. <p>
   */

  final int getConnectionId()
  {
    return id;
  }

  /** Set the objid. Private, used only by the server. */

  final void setConnectionId(int id)
  {
    this.id = id;
  }

  /** Ask FTS to Undo the connection. */

  public void delete()
  {
    if (deleted)
      return;

    deleted = true;

    Fts.getSelection().removeConnection(this);

    from.setDirty();
	
    Fts.getServer().deleteConnection(this);
  }


  /** Undo the connection, only the client part;
   *  indirectly called by FTS.
   */

 void release()
  {
    deleted = true;

    from.setDirty(); // from and to must be in the same document !!

    if (from.getPatcherData() != null)
      from.getPatcherData().removeConnection(this);

    // Clean up

    from = null;
    to = null;
  }

  /** Access the From. The From is the FtsObject origin of the connection. */

  public FtsObject getFrom()
  {
    return from;
  }

  /** Access the To. The To is the FtsObject destination of the connection. */

  public FtsObject getTo()
  {
    return to;
  }

  /** Access the From outlet. */

  public int getFromOutlet()
  {
    return outlet;
  }

  /** Access the To inlet. */

  public int getToInlet()
  {
    return inlet;
  }

  public String  toString()
  {
    return "FtsConnection(" + from + "," + outlet + "," + to + "," + inlet + ", #" + id + ")";
  }
}






