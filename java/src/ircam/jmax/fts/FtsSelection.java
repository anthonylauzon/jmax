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
import ircam.jmax.utils.*;

/**
 * Implement the selection; for now, without listener, and
 * local to each patcher; later, unique, and more integrated with FTS.
 * 
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

  protected  FtsSelection(FtsObject parent, String className, String description, int objId)
  {
    super(parent, className, null, description, objId);
  }

  /** Add an object to this container  */

  final public void addObject(FtsObject obj)
  {
    objects.addElement(obj);

    Fts.getServer().sendObjectMessage(this, -1, "add_object", obj);
  }

  /** Remove an object from this container. */

  final public void removeObject(FtsObject obj)
  {
    if (objects.contains(obj))
      {
	objects.removeElement(obj);
	Fts.getServer().sendObjectMessage(this, -1, "remove_object", obj);
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

    Fts.getServer().sendObjectMessage(this, -1, "add_connection", obj);
  }

  /** Remove an connection from this container. */

  final public void removeConnection(FtsConnection obj)
  {
    if (connections.contains(obj))
      {
	connections.removeElement(obj);
	Fts.getServer().sendObjectMessage(this, -1, "remove_connection", obj);
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






