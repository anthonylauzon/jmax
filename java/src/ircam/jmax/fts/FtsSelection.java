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
import ircam.jmax.utils.*;

/**
 * Implement the selection.
 * Only one instance for server is built.
 */

public class FtsSelection  extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /** The objects contained in the patcher */

  private MaxVector objects     = new MaxVector();

  /** All the connections between these objects */

  private MaxVector connections = new MaxVector();

  /**
   * Create a Fts selection;
   */

  protected FtsSelection(Fts fts, FtsObject parent, String className, String description, int objId)
  {
    super(fts, parent, className, null, description, objId);
  }

  /** Add an object to this container  */

  final public void addObject(FtsObject obj)
  {
    objects.addElement(obj);

    fts.getServer().sendObjectMessage(this, -1, "add_object", obj);
  }

  /** Remove an object from this container. */

  final public void removeObject(FtsObject obj)
  {
    if (objects.contains(obj))
      {
	objects.removeElement(obj);
	fts.getServer().sendObjectMessage(this, -1, "remove_object", obj);
      }
  }

  /** Get the objects */

  final public MaxVector getObjects()
  {
    return objects;
  }

  /** Add a connection to this selection. */

  final public void addConnection(FtsConnection obj)
  {
    connections.addElement(obj);

    fts.getServer().sendObjectMessage(this, -1, "add_connection", obj);
  }

  /** Remove an connection from this container. */

  final public void removeConnection(FtsConnection obj)
  {
    if (connections.contains(obj))
      {
	connections.removeElement(obj);
	fts.getServer().sendObjectMessage(this, -1, "remove_connection", obj);
      }
  }

  /** Get the connections */

  final public MaxVector getConnections()
  {
    return connections;
  }

  /** clean: remove everything */

  final public void clean()
  {
    sendMessage(-1, "clear", null);

    objects.removeAllElements();
    connections.removeAllElements();
  }
}






