package ircam.jmax.fts;

import java.io.*;
import java.util.*;
import tcl.lang.*;

import ircam.jmax.*;

/** 
 *  This is the super class of all
 * objects that include other objects, like
 * selections, patchers, template and abstractions.
 *
 * An object or connections can belong to multiple
 * abstract containers (selections, for example); a abstract container
 * is not necessarly represented in FTS, and do not have inlets, outlets,
 * and do not necessarly represent a patcher in the FTS meaning.
 *
 * I.e. an abstract container cannot be a parent of an object, for this
 * you need a FtsContainerObject.
 */

abstract public class FtsAbstractContainerObject extends FtsObject
{
  /** The objects contained in the patcher */

  Vector objects     = new Vector();

  /** All the connections between these objects */

  Vector connections = new Vector();

  /** Just for the building of the root object */

  FtsAbstractContainerObject()
  {
    super();
  }

  protected  FtsAbstractContainerObject(FtsContainerObject parent, String className, String description)
  {
    super(parent, className, description);
  }


  /** Add an object to this container  */

  final void addObjectToContainer(FtsObject obj)
  {
    objects.addElement(obj);
    put("newObject", obj); // the newObject property keep the last object created
  }

  /** Remove an object from this container. */

  final void removeObjectFromContainer(FtsObject obj)
  {
    objects.removeElement(obj);
  }

  /** Replace an object with an other one; 
   * Cannot be called for inlets and outlets, only
   * for real standard object
   */

  void replace(FtsObject oldObject, FtsObject newObject)
  {
    // replace it in the object list

    removeObjectFromContainer(oldObject);
    addObjectToContainer(newObject);

    // replace it in all the connections

    for (int i = 0; i < connections.size(); i++)
      ((FtsConnection)connections.elementAt(i)).replace(oldObject, newObject);
  }


  /** Add an connection to this container. */

  final void addConnectionToContainer(FtsConnection obj)
  {
    connections.addElement(obj);
    put("newConnection", obj); // the newConnection property keep the last connection created
  }

  /** Remove an connection from this container. */

  final void removeConnectionFromContainer(FtsConnection obj)
  {
    connections.removeElement(obj);
  }

  /** Get the connections. */

  public final Vector getConnections()
  {
    return connections;
  }

  /** Get the objects. */

  public final Vector getObjects()
  {
    return objects;
  }
}







