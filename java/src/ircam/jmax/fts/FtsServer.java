package ircam.jmax.fts;

import java.util.*;
import java.io.*;

/**
 * Represent the FTS server we are connected to. <br>
 * All the meaning full operations on the server are implemented
 * thru methods of this class; together with the FtsConnection class
 * it represent the lower level of the application layer. <p>
 *
 * The FtsClass is specialized by classes providing the physical connection
 * to the actual server. <p>
 *
 * This class provide a method for each kind message can be sent to FTS;
 * this is done to optimize memory use of FTS, and to optimize speed in outgoing
 * messages.
 */


public class FtsServer 
{
  // Code handling the global server and the connection 

  static FtsServer theServer = null;

  static public FtsServer getServer()
  {
    return theServer;
  }
  
  public static void connectToFts(String theFtsdir, String theFtsname, String mode, String server, String port)
  {
    if (mode.equals("socket")) 
      theServer = new FtsSocketServer(server, Integer.parseInt(port));
    else if (mode.equals("client"))
      theServer = new FtsSocketClientServer(server);
    else if (mode.equals("local"))
      theServer = new FtsSubProcessServer();
    else
      System.out.println("unknown FTS connection type "+mode+": can't connect to FTS");
    theServer.setParameter("ftsdir", theFtsdir);
    theServer.setParameter("ftsname", theFtsname);
    theServer.start();
  }

  /** Count objects for statistics */

  int objCount = 0;

  /** Count connections for statistics */

  int connCount = 0;

  /** The FtsPort used to communicate with FTS */

  FtsPort port;

  /** The name of this server, used for printouts only */

  String name;

  /** The root object of this server */

  FtsContainerObject root;

  /** The list of listener of update groups */
  
  Vector updateGroupListeners;

  /** Create an FTS Server. With a given port. */

  FtsServer(String name, FtsPort port)
  {
    this.name = name;
    this.port = port;

    this.port.setServer(this);
  }

  /** Give a string representation of the server */

  public String toString()
  {
    return this.getClass().getName() + "<" + name + ">";
  }

  /** Get the server root object.
   * Every server have a root object, that is a patcher
   * that contains all the top level patchers; this root object
   * is not editable, and represent the global scope for FTS.
   */ 

  public FtsContainerObject getRootObject()
  {
    return root;
  }

  /** Start the server. */

  public void start()
  {
    port.start();

    // Build the root object and the root patcher

    root = FtsPatcherObject.makeRootObject(this);
  }

  /** Stop the server. */

  public void stop()
  {
    port.close();
  }

  /** Set a server parameter, <i>before</i> the server start. */

  public void setParameter(String name, String value)
  {
    port.setParameter(name, value);
  }


  /** Set the flushing mode; set this to true during loading,
   *  false during interaction. Sync ask for a flush, to avoid deadlocks.
   */

  public void setFlushing(boolean flushing)
  {
    if (port != null)
      port.setFlushing(flushing);
  }

  /** Send a "open patcher" messages to FTS.*/

  final void savePatcher(FtsObject patcher, String filename)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_save_patcher_cmd);
	port.sendObject(patcher);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "open patcher" messages to FTS.*/

  final void openPatcher(FtsObject patcher)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_open_patcher_cmd);
	port.sendObject(patcher);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "close patcher" messages to FTS.*/

  final void closePatcher(FtsObject patcher)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_close_patcher_cmd);
	port.sendObject(patcher);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "patcher loaded" messages to FTS.*/

  final void patcherLoaded(FtsObject patcher)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_patcher_loaded_cmd);
	port.sendObject(patcher);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "new object" messages to FTS; receive the class Name and a description as a string;
   *  It is actually used only for those objects that don't have the class name in the description,
   *  i.e. messages.
   */

  final  void newObject(FtsObject patcher, FtsObject obj, String className, String description)
  {
    try
      {
	registerObject(obj);
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	port.sendString(className);
	FtsParseToPort.parseAndSendObject(description, port);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "new object" messages to FTS; receive a complete description as a string;
   *   Used for all the objects that have the class identity in the string description
   */

  final  void newObject(FtsObject patcher, FtsObject obj, String description)
  {
    try
      {
	registerObject(obj);
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!

	FtsParseToPort.parseAndSendObject(description, port);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "new object" messages to FTS,  specialized version to define a patcher
   */

  final  void newPatcherObject(FtsObject patcher, FtsObject obj, String name, int ninlets, int noutlets)
  {
    try
      {
	registerObject(obj);
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	port.sendString("patcher");
	port.sendString(name);
	port.sendInt(ninlets);
	port.sendInt(noutlets);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "new inlet object" messages to FTS, without the inlet number */

  final void newInletObject(FtsObject patcher, FtsInletObject obj)
  {
    try
      {
	registerObject(obj);
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	port.sendString("inlet");
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "new inlet object" messages to FTS, with the inlet number */

  final void newInletObject(FtsObject patcher, FtsInletObject obj, int pos)
  {
    try
      {
	registerObject(obj);
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	port.sendString("inlet");
	port.sendInt(pos);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "new outlet object" messages to FTS, without the outlet number */

  final void newOutletObject(FtsObject patcher, FtsOutletObject obj)
  {
    try
      {
	registerObject(obj);
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	port.sendString("outlet");
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "new outlet object" messages to FTS, with the outlet number */

  final void newOutletObject(FtsObject patcher, FtsOutletObject obj, int pos)
  {
    try
      {
	registerObject(obj);
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(obj.getObjId());
	port.sendString("outlet");
	port.sendInt(pos);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "replace object" messages to FTS.*/

  final void replaceObject(FtsObject oldObject, FtsObject newObject)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_replace_object_cmd);
	port.sendObject(oldObject);
	port.sendObject(newObject);
	port.sendEom();

	// In FTS, they exchanged their ID identity; 
	// we exchange their IDs on the client side to keep
	// consistency

	int idx;

	idx = oldObject.getObjId();
	oldObject.setObjId(newObject.getObjId());
	newObject.setObjId(idx);
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "redefine object" messages to a patcher in FTS.
   *  Special optimized version for patcher loading/editing
   */

  final void redefinePatcherObject(FtsObject obj, String name, int ninlets, int noutlets)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	port.sendObject(obj);
	port.sendString("patcher");
	port.sendString(name);
	port.sendInt(ninlets);
	port.sendInt(noutlets);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Specialized version for inlets */

  final void redefineInletObject(FtsObject obj, int pos)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	port.sendObject(obj);
	port.sendString("inlet");
	port.sendInt(pos);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Specialized version for outlets */

  final void redefineOutletObject(FtsObject obj, int pos)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	port.sendObject(obj);
	port.sendString("outlet");
	port.sendInt(pos);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Specialized version for message objects */

  final void redefineMessageObject(FtsObject obj, String description)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	port.sendObject(obj);
	port.sendString("messbox");

	FtsParseToPort.parseAndSendObject(description, port);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "free object" messages to FTS.*/

  final void freeObject(FtsObject obj)
  {
    if (obj.getObjId() != -1)
      {
	try
	  {
	    unregisterObject(obj);
	    port.sendCmd(FtsClientProtocol.fts_free_object_cmd);
	    port.sendObject(obj);
	    port.sendEom();
	  }
	catch (java.io.IOException e)
	  {
	  }
      }
  }

  /** Send an "object message" messages to FTS.*/

  final void sendObjectMessage(FtsObject dst, int inlet, String selector, Vector args)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_message_cmd);
	port.sendObject(dst);
	port.sendInt(inlet);
	port.sendString(selector);

	if (args != null)
	  port.sendVector(args);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send an "_set" messages to inlet 0 of an FTS object with as arguments elements
    from an int arrays; and have a special function to avoid building 
    a big Vector of arguments; special for the Table object, may be sustituted by
    something more generic in the future.
    */

  final void sendSetMessage(FtsObject dst, int offset, int[] values, int from, int to)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_message_cmd);
	port.sendObject(dst);
	port.sendInt(0);
	port.sendString("_set");
	port.sendInt(offset);

	if (values != null)
	  port.sendArray(values, from, to);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "named object message" messages to FTS.
    @deprecated
   */

  final public void sendNamedObjectMessage(String dst, int inlet, String selector, Vector args)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_named_message_cmd);

	port.sendString(dst);
	port.sendInt(inlet);
	port.sendString(selector);

	if (args != null)
	  port.sendVector(args);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "connect objects" messages to FTS. */

  final void connectObjects(FtsObject from, int outlet, FtsObject to, int inlet)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_connect_objects_cmd);
	port.sendObject(from);
	port.sendInt(outlet);
	port.sendObject(to);
	port.sendInt(inlet);
	port.sendEom();

	connCount++;
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "disconnect objects" messages to FTS. */

  final void disconnectObjects(FtsObject from, int outlet, FtsObject to, int inlet)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_disconnect_objects_cmd);
	port.sendObject(from);
	port.sendInt(outlet);
	port.sendObject(to);
	port.sendInt(inlet);
	port.sendEom();

	connCount--;
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "put property" messages to FTS. */

  final void putObjectProperty(FtsObject object, String name, Object value)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_put_property_cmd);
	port.sendObject(object);
	port.sendString(name);
	port.sendValue(value);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "get property" messages to FTS. */

  final void getObjectProperty(FtsObject object, String name)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_get_property_cmd);
	port.sendObject(object);
	port.sendString(name);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "ucs" messages to FTS. */

  final public void ucsMessage(Vector args)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.ucs_cmd);
	port.sendVector(args);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /**
   * Sync point with FTS.
   * Send a ping message, and sychroniuosly
   * wait for FTS to answer; this guarantee that all the
   * pending callbacks and updates from FTS have been processed,
   * and all the values are consistents with the FTS counterpart.
   * Should probabily be available elsewhere, and not in this class.
   */

  final public synchronized void syncToFts()
  {
    try
      {
	port.sendCmd(FtsClientProtocol.sync_cmd);
	port.sendEom();
	port.flush();
      }

    catch (java.io.IOException e)
      {
      }

    waitForPong();
  }


  /** Send a generic message to FTS. May be this is not used.*/

  final void sendMessage(FtsMessage msg)
  {
    port.sendMessage(msg);
  }


  /** Handle adding an update group listener */

  public void addUpdateGroupListener(FtsUpdateGroupListener listener)
  {
    if (updateGroupListeners == null)
      updateGroupListeners = new Vector();
    
    updateGroupListeners.addElement(listener);
  }

  /** Handle removing an update group listener */

  public void removeUpdateGroupListener(FtsUpdateGroupListener listener)
  {
    if (updateGroupListeners == null)
      return;

    updateGroupListeners.removeElement(listener);
  }

  /**
   * Dispatch a server message.
   * Note that the Server object is
   * now protocol dependent, so we don't attempt to make the protocol
   * handling extensible; adding a piece of protocol means to add
   * a piece of code here; this will allow for a more optimized
   * parser (less dynamic allocation ...)
   */

  void dispatchMessage(FtsMessage msg)
  {
    switch (msg.getCommand())
      {
      case FtsClientProtocol.fts_property_value_cmd:
	{
	  FtsObject obj;

	  obj = (FtsObject) msg.getArgument(0);
	  obj.localPut((String) msg.getArgument(1), 
		       msg.getArgument(2));

	}
      break;

      case FtsClientProtocol.fts_server_message_cmd:
	{
	  FtsObject obj;

	  obj = (FtsObject) msg.getArgument(0);
	  obj.handleMessage(msg);
	}
      break;

      case FtsClientProtocol.post_cmd:
	System.out.print(msg.getArgument(0)); // print the first argument
	break;

      case FtsClientProtocol.sync_done_cmd:
	deliverPong();
	break;
	
      case FtsClientProtocol.fts_update_group_start_cmd:
	if (updateGroupListeners != null)
	  for (int i = 0; i < updateGroupListeners.size(); i++)
	    ((FtsUpdateGroupListener) updateGroupListeners.elementAt(i)).updateGroupStart();
	break;

      case FtsClientProtocol.fts_update_group_end_cmd:
	if (updateGroupListeners != null)
	  for (int i = 0; i < updateGroupListeners.size(); i++)
	    ((FtsUpdateGroupListener) updateGroupListeners.elementAt(i)).updateGroupEnd();
	break;

      default:
	break;
      }
  }

  /** Synchronization primitive for the Ping/Pong protocol. */

  private synchronized void waitForPong()
  {
    try
      {
	// naive solution to the FTS crash problem
	// it should be smarter, detect the problem,
	// and set the server as halted.

	wait(10000);
      }
    catch (java.lang.InterruptedException e)
      {
	// ignore iand continue
      }
  }

  /** Synchronization primitive for the Ping/Pong protocol. */

  private synchronized void deliverPong()
  {
    notifyAll();
  }


  /* ID HANDLING */


  /**
   * The server ID Counter.
   *
   * Used to generate IDs for FTS; skip zero, 
   * and continue by increments; do not reuse the Ids,
   * in this implementation.
   */

  private int ftsIDCounter = 1;	// skip zero

  /** The object table. Used to map back Ids to objects. */

  private Vector objTable = new Vector();

  /** The max Id registered */

  private int maxId = 0;

  /**
   * ID handling.
   * Each server instance have it own object table, and provide
   * a register and unregister function; also, the server store and access
   * the object id directly in the FtsObject, for efficency, but this is 
   * transparent to the object.
   */

  private void registerObject(FtsObject obj)
  {
    int newid;

    newid = ftsIDCounter++;

    if (newid > maxId)
      {
	maxId = (newid * 3) / 2;
	objTable.setSize(maxId + 1);
      }

    objTable.setElementAt(obj, newid);

    obj.setObjId(newid);

    // stats
    objCount++;
  }

  /**
   * ID handling.
   * Each server instance have it own object table, and provide
   * a register and unregister function; also, the server store and access
   * the object id directly in the FtsObject, for efficency, but this is 
   * transparent to the object.
   */

  private void unregisterObject(FtsObject obj)
  {
    objTable.setElementAt(null, obj.getObjId());
    objCount--;
  }

  /**
   * Get an FtsObject by its FtsId (used only by the FAL).
   *
   * @deprecated
   */

  FtsObject getObjectByFtsId(int id)
  {
    if (id == 0)
      return null;
    else
      return (FtsObject) objTable.elementAt(id);
  }
}







