package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
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
 * A container is both a MaxData and a FtsObjectWithData; of course, its data is itself.
 */

abstract public class FtsContainerObject extends FtsObject implements MaxData, FtsObjectWithData
{
  /** The objects contained in the patcher */

  private Vector objects     = new Vector();

  /** All the connections between these objects */

  private Vector connections = new Vector();

  /** True if the patcher is Open in FTS */

  protected boolean open = false;

  /** True if the patcher content have been downloaded from FTS
   *  or anyway consistent because built in the editor
   */

  protected boolean downLoaded = false;

  protected  FtsContainerObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);
  }

  /** Tell MDA to dispose the editor on this data */

  public void delete()
  {
    Mda.dispose(this);
    super.delete();
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

  public final Vector getObjects()
  {
    if (! downLoaded)
      download();

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

  public final Vector getConnections()
  {
    if (! downLoaded)
      download();

    return connections;
  }

  /** Open tell FTS that this patcher is "alive";
   * open need to download the patcher if not downloaded.
   */

  public final void open()
  {
    if (! downLoaded)
      download();

    open = true;
    Fts.getServer().openPatcher(this);
    Fts.sync();
  }

  /** Close tell FTS that this patcher is not "alive". */

  public final void close()
  {
    open = false;

    Fts.getServer().closePatcher(this);
  }

  /** Check if the patcher is open. */

  public final boolean isOpen()
  {
    return open;
  }

  /** Download the patcher content, if needed */

  private final void download()
  {
    if (! downLoaded)
      Fts.getServer().sendDownloadPatcherAndSync(this);

    downLoaded = true;
  }

  /**
   * Download the patcher content; if it was already downloaded, 
   * actually do an incremental download of new objects
   */

  final void redownload()
  {
    Fts.getServer().sendDownloadPatcherAndSync(this);
    downLoaded = true;
  }

  /** Call this method to tell the application layer
    that this object has been created from the editor,
    and so it is implicitly downloaded (i.e. consistent
    with FTS */

  public void setDownloaded()
  {
    downLoaded = true;
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


  /*****************************************************************************/
  /*                                                                           */
  /*                    Object Naming                                          */
  /*                                                                           */
  /*****************************************************************************/

  /** Get an object by name; a name is either a single name or a composed
   *  name; a composed name is interpreted relatively to this container.
   */


  public FtsObject getObjectByName(String name)
  {
    if (name.indexOf('.') == -1)
      return getObjectBySimpleName(name);
    else
      {
	String rootName;
	String tailName;
	FtsObject obj;
	
	rootName = name.substring(0, name.indexOf('.'));
	tailName = name.substring(name.indexOf('.') + 1);

	obj = getObjectBySimpleName(rootName);

	if (obj instanceof FtsContainerObject)
	  return ((FtsContainerObject) obj).getObjectByName(tailName);
	else
	  return null;
      }
  }

  final private FtsObject getObjectBySimpleName(String name)
  {
    for (int i = 0; i < getObjects().size(); i++)
      {
	FtsObject obj   =  (FtsObject) getObjects().elementAt(i);
	String objName  =  (String) obj.get("name");

	if ((objName != null) && (objName.equals(name)))
	  return obj;
      }

    return null;
  }

  /// MaxData implementation

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

  public String getName()
  {
    return getObjectName();
  }

  // FtsObjectWithData implementation

  public MaxData getData()
  {
    return this;
  }

  public void setData(MaxData data) throws FtsException
  {
    throw new FtsException(new FtsError(FtsError.ILLEGAL_OPERATION, "Cannot set the content of a  patcher"));
  }
}







