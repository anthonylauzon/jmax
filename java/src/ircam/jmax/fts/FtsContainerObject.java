package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.mda.*;

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

  /** Remove the  object from this container.
    Connections are not touched, so they should be cleanup before.
    */

  final void removeObjectFromContainer(FtsObject obj)
  {
    localPut("deletedObject", obj); // the deleteObject property keep the last object deleted
    objects.removeElement(obj);
  }

  /** Access the objects contained in the container.
   */

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
}







