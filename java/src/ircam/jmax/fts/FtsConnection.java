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
// Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
// 

package ircam.jmax.fts;

import ircam.jmax.*;
import java.io.*;
import java.util.*;
import ircam.ftsclient.*;

/**
 * The proxy of an Fts connection.
 * Deals with the connection creation/deletion and saving/loading.
 */

public class FtsConnection extends FtsObject implements Serializable
{
  /* numbering doubled on server side (mess_types.h) */
  public static final int fts_connection_invalid = 0; /* from error object or type missmatch */
  public static final int fts_connection_anything = 1; /* message which is not one of the following */
  public static final int fts_connection_atom = 2; /* single atom (value) */
  public static final int fts_connection_object = 3; /* objects */
  public static final int fts_connection_signal = 4; /* signal connection */

  private transient int id;

  protected transient FtsConnectionListener listener;
  
  private transient boolean deleted = false; 
  FtsGraphicObject from;
  int outlet;

  FtsGraphicObject to;
  int inlet;

  int type;

  /** Create a FTS connection instance.
   * The FTS side of the connection is created in the Fts class.
   * @see ircam.jmax.fts.Fts#makeFtsConnection
   */

  FtsConnection(FtsServer server, FtsPatcherObject patcher, int id, FtsGraphicObject from, 
		int outlet, FtsGraphicObject to, int inlet, int type)
  {
    super(server, patcher, id);
    
    this.from   = from;
    this.outlet = outlet;
    this.to     = to;
    this.inlet  = inlet;
    this.type   = type;

    if (patcher != null)
	patcher.addConnection(this);
  }

  FtsConnection(FtsServer server, FtsPatcherObject patcher, int id, FtsGraphicObject from, 
		int outlet, FtsGraphicObject to, int inlet)
  {
      this(server, patcher, id, from, outlet, to, inlet, fts_connection_anything);
  }

  /** Set the unique object listener */
  public void setConnectionListener(FtsConnectionListener obj)
  {
      listener = obj;
  }

  /** Get the current object listener */
  public FtsConnectionListener getConnectionListener()
  {
    return listener;
  }

  void redefine(FtsGraphicObject from, int outlet, FtsGraphicObject to, int inlet, int type)
  {
    this.from   = from;
    this.outlet = outlet;
    this.to     = to;
    this.inlet  = inlet;
    this.type   = type;

    if(listener!=null)
	listener.typeChanged(type);
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

    ircam.jmax.editors.patcher.ErmesSelection.getFtsSelection().removeConnection(this);

    from.setDirty();
	
    /*fts.getServer().deleteConnection(this);*/
    ((FtsPatcherObject)from.getParent()).requestDeleteConnection(this);
  }


  /** Undo the connection, only the client part.
   *  indirectly called by FTS.
   */

 void release()
  {
    deleted = true;

    from.setDirty(); // from and to must be in the same document !!

    if ((from.getParent() != null)&&(from.getParent() instanceof FtsPatcherObject))
	((FtsPatcherObject)from.getParent()).removeConnection(this);

    // Clean up

    from = null;
    to = null;
  }

  /** Access the From. The From is the FtsObject origin of the connection. */

  public FtsGraphicObject getFrom()
  {
    return from;
  }

  /** Access the To. The To is the FtsObject destination of the connection. */

  public FtsGraphicObject getTo()
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

  public int getType()
  {
    return type;
  }

  /** Get a string debug representation for the connection */

  public String  toString()
  {
    return "FtsConnection(" + from + "," + outlet + "," + to + "," + inlet + ", #" + id + ", <" + type + ">)";
  }
}


