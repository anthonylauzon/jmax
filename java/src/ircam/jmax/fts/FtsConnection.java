//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import ircam.jmax.*;
import java.io.*;
import java.util.*;

/**
 * The proxy of an Fts connection.
 * Deals with the connection creation/deletion and saving/loading.
 */

public class FtsConnection 
{
  Fts  fts; // the server this connection belong to.

  private int id;

  boolean deleted = false; 
  FtsObject from;
  int outlet;

  FtsObject to;
  int inlet;

  /** Create a FTS connection instance.
   * The FTS side of the connection is created in the Fts class.
   * @see ircam.jmax.fts.Fts#makeFtsConnection
   */

  FtsConnection(Fts fts,
		FtsPatcherData data, int id, FtsObject from, int outlet, FtsObject to, int inlet)
  {
    super();

    this.fts    = fts;
    this.id     = id;
    this.from   = from;
    this.outlet = outlet;
    this.to     = to;
    this.inlet  = inlet;

    if (data != null)
      data.addConnection(this);
  }

  /** Locally redefine a connection */

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

    fts.getSelection().removeConnection(this);

    from.setDirty();
	
    fts.getServer().deleteConnection(this);
  }


  /** Undo the connection, only the client part.
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

  /** Get a string debug representation for the connection */

  public String  toString()
  {
    return "FtsConnection(" + from + "," + outlet + "," + to + "," + inlet + ", #" + id + ")";
  }
}






