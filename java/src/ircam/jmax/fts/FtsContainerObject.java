package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.mda.*;

import com.sun.java.swing.*;

/** 
 *  This is the super class of all
 * objects that include other objects, like
 * patchers,  abstractions.
 * 
 * This is a specialization of the FtsAbstractContainer that 
 * necessarly represent a patcher in the FTS meaning; i.e. 
 * a real patcher or abstraction instance.
 * So it add inlets/outlets housekeeping, patcher opening, patcher after load init,
 * patcher relative properties, script evaluation, .pat file support, find and object
 * naming support.
 * Removed the inlets/outlets housekeeping: since the .pat parsing is done in FTS
 * we don't need to make them different from the other objects.
 * 
 */

abstract public class FtsContainerObject extends FtsObject implements MaxData
{
  /** The objects contained in the patcher */

  private MaxVector objects     = new MaxVector();

  /** All the connections between these objects */

  private MaxVector connections = new MaxVector();

  protected  FtsContainerObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);
  }


  public void release()
  {
    super.release();
    objects = null;
    connections = null;
  }

  /*
   * Object handling
   */

  /** Add an object to this container  */

  final void addObjectToContainer(FtsObject obj)
  {
    objects.addElement(obj);
    localPut("newObject", obj); // the newObject property keep the last object created
  }

  /** Remove the connections arriving or starting from an object
    from this container. */

  final void removeObjectConnectionsFromContainer(FtsObject obj)
  {
    // First, look in the connections, and collect the connections
    // to be deleted then delete them
    // WARNING: doing like that *because* FtsConnection.delete change
    // the connections vector, so the loop would not work.

    MaxVector toDelete = new MaxVector();

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
  }

  /** Remove the  object from this container.
    Connections are not touched, so they should be cleanup before.
    */

  final void removeObjectFromContainer(FtsObject obj)
  {
    localPut("deletedObject", obj); // the deleteObject property keep the last object deleted
    objects.removeElement(obj);
  }

  /** Overwrite the getObjects methods so to download the patcher
    by need */

  public final MaxVector getObjects()
  {
    return objects;
  }

  /*
   * Connections handling
   */

  /** Add an connection to this container. */

  final void addConnectionToContainer(FtsConnection obj)
  {
    connections.addElement(obj);
    localPut("newConnection", obj); // the newConnection property keep the last connection created
  }

  /** Remove an connection from this container. */

  final void removeConnectionFromContainer(FtsConnection obj)
  {
    localPut("deletedConnection", obj); // the deleteConnection property keep the last connection deleted
    connections.removeElement(obj);
  }

  /** Overwrite the getConnections methods so to download the patcher
    by need */

  public final MaxVector getConnections()
  {
    return connections;
  }

  /** Close tell FTS that this patcher is  "alive". */

  public final void startUpdates()
  {
    Fts.getServer().openPatcher(this);
  }


  /** Close tell FTS that this patcher is not "alive". */

  public final void stopUpdates()
  {
    Fts.getServer().closePatcher(this);
  }

  /**
   * Download the patcher content; if it was already downloaded, 
   * actually do an incremental download of new objects
   */

  final void redownload()
  {
    Fts.getServer().sendDownloadPatcher(this);
    Fts.sync();
  }

  /**
   * Loaded tell FTS that this patcher has been loaded, and
   *  post-load initialization should be fired.
   */

  public final void loaded()
  {
    Fts.getServer().patcherLoaded(this);
  }

  /** Keep the container handlers */

  FtsObject.PropertyHandlerTable containerPropertyHandlerTable = null;

  /** Watch a property of all the object in the patcher 
   *
   */

  public void watchAll(String property, FtsPropertyHandler handler)
  {
    watchAll(property, handler, handler);
  }

  /** Watch a property of all the object in the patcher 
   *
   */

  public void watchAll(String property, FtsPropertyHandler handler, Object owner)
  {
    if (containerPropertyHandlerTable == null)
      containerPropertyHandlerTable = new PropertyHandlerTable();
    
    containerPropertyHandlerTable.watch(property, handler, owner);
  }

  /** Remove a watch all */

  public void removeWatchAll(Object owner)
  {
    if (containerPropertyHandlerTable != null)
      containerPropertyHandlerTable.removeWatch(owner);
  }

  /** execute the watch all */

  void callWatchAll(String property, Object value, Object author)
  {
    if (containerPropertyHandlerTable != null)
      containerPropertyHandlerTable.callHandlers(property, value, author);
  }

  /* ask fts to send back the value property (and so call the handlers) 
     for the objects in a patcher.
   */
     
  public void askAll(String name)
  {
    Fts.getServer().askAllObjectProperty(this, name);
  }


  /** The Max Document his container belong to, or null in case
   *  we should ask the parent
   */

  FtsPatcherDocument document;

  public MaxDocument getDocument()
  {
    if (document == null)
      {
	if (parent != null)
	  return parent.getDocument();
	else
	  return null;
      }
	    
    else
      return (MaxDocument) document;
  }

  public void setDocument(MaxDocument document)
  {
    this.document = (FtsPatcherDocument) document;
  }
}







