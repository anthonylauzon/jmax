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

  private Vector objects     = new Vector();

  /** All the connections between these objects */

  private Vector connections = new Vector();

  /** Just for the building of the root object */

  FtsAbstractContainerObject()
  {
    super();
  }

  FtsAbstractContainerObject(int objId)
  {
    super(objId);
  }

  protected  FtsAbstractContainerObject(FtsContainerObject parent, String className, String description)
  {
    super(parent, className, description);
  }

  protected  FtsAbstractContainerObject(FtsContainerObject parent, String className,
					String description, int objId)
  {
    super(parent, className, description, objId);
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
    put("deletedObject", obj); // the deleteObject property keep the last object deleted

    // First, look in the connections, and collect the connections
    // to be deleted then delete them
    // WARNING: doing like that *because* FtsConnection.delete change
    // the connections vector, so the loop would not work.

    Vector toDelete = new Vector();

    for (int i = 0; i < connections.size() ; i++)
      {
	FtsConnection c;

	c = (FtsConnection) connections.elementAt(i);
	
	if ((c.from == obj) || (c.to == obj))
	  toDelete.addElement(c);
      }

    for (int i = 0; i < toDelete.size() ; i++)
      {
	FtsConnection c;

	c = (FtsConnection) toDelete.elementAt(i);
	
	if ((c.from == obj) || (c.to == obj))
	  c.delete();
      }

    objects.removeElement(obj);
  }

  /** Replace an object with an other one in all the connections
   */

  void replaceInConnections(FtsObject oldObject, FtsObject newObject)
  {    
    // delete the connections that no more consistent
    // First collect them in a aux vector, then remove them
    // It is slow, but safer because removing elements in vector
    // shift the content

    Vector toDelete = new Vector();

    for (int i = 0; i < connections.size(); i++)
      {
	FtsConnection conn = (FtsConnection)connections.elementAt(i);

	if (! (conn.checkConsistency()))
	  toDelete.addElement(conn);
      }

    for (int i = 0; i < toDelete.size(); i++)
      {
	FtsConnection conn = (FtsConnection)connections.elementAt(i);

	conn.delete();
      }

    // replace it in all the survived connections

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
    put("deleteConnection", obj); // the deleteConnection property keep the last connection deleted
    connections.removeElement(obj);
  }

  /** Get the connections. */

  public Vector getConnections()
  {
    return connections;
  }

  /** Get the objects. */

  public Vector getObjects()
  {
    return objects;
  }
}







