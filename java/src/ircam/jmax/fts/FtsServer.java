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
  /** Count objects for statistics */

  int objCount = 0;

  /** Count connections for statistics */

  int connCount = 0;

  /** The FtsPort used to communicate with FTS */

  FtsPort connection;

  /** The name of this server, used for printouts only */

  String name;

  /** The root object of this server */

  FtsContainerObject root;

  /** Create an FTS Server. With a given connection. */

  FtsServer(String name, FtsPort connection)
  {
    this.name = name;
    this.connection = connection;

    this.connection.setServer(this);
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
    connection.start();

    // Build the root object and the root patcher

    root = FtsPatcherObject.makeRootObject(this);
  }

  /** Stop the server. */

  public void stop()
  {
    connection.close();
  }

  /** Set a server parameter, <i>before</i> the server start. */

  public void setParameter(String name, String value)
  {
    connection.setParameter(name, value);
  }

  /** Send a "open patcher" messages to FTS.*/

  final void openPatcher(FtsObject patcher)
  {
    try
      {
	connection.sendCmd(FtsClientProtocol.fts_open_patcher_cmd);
	connection.sendObject(patcher);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_close_patcher_cmd);
	connection.sendObject(patcher);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_patcher_loaded_cmd);
	connection.sendObject(patcher);
	connection.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "new object" messages to FTS; receive the class Name and a description as a string;
   *  It is actually used only for those objects that don't have the class name in the description,
   *  i.e. messages and (temporarly) comments.
   */

  final  void newObject(FtsObject patcher, FtsObject obj, String className, String description)
  {
    Vector args;

    args = new Vector();
    
    FtsParse.parseObjectArgs(description, args);

    try
      {
	registerObject(obj);
	connection.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	connection.sendObject(patcher);
	connection.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	connection.sendString(className);

	if (args != null)
	  connection.sendVector(args);

	connection.sendEom();
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
    String className;
    Vector args;

    args = new Vector();
    
    className = FtsParse.parseObject(description, args);

    try
      {
	registerObject(obj);
	connection.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	connection.sendObject(patcher);
	connection.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	connection.sendString(className);

	if (args != null)
	  connection.sendVector(args);

	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	connection.sendObject(patcher);
	connection.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	connection.sendString("patcher");
	connection.sendString(name);
	connection.sendInt(ninlets);
	connection.sendInt(noutlets);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	connection.sendObject(patcher);
	connection.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	connection.sendString("inlet");
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	connection.sendObject(patcher);
	connection.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	connection.sendString("inlet");
	connection.sendInt(pos);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	connection.sendObject(patcher);
	connection.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	connection.sendString("outlet");
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	connection.sendObject(patcher);
	connection.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	connection.sendString("outlet");
	connection.sendInt(pos);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_replace_object_cmd);
	connection.sendObject(oldObject);
	connection.sendObject(newObject);
	connection.sendEom();

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
	connection.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	connection.sendObject(obj);
	connection.sendString("patcher");
	connection.sendString(name);
	connection.sendInt(ninlets);
	connection.sendInt(noutlets);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	connection.sendObject(obj);
	connection.sendString("inlet");
	connection.sendInt(pos);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	connection.sendObject(obj);
	connection.sendString("outlet");
	connection.sendInt(pos);
	connection.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Specialized version for message objects */

  final void redefineMessageObject(FtsObject obj, String description)
  {
    Vector args;
    
    args = new Vector();
    
    FtsParse.parseObjectArgs(description, args);

    try
      {
	connection.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	connection.sendObject(obj);
	connection.sendString("messbox");

	if (args != null)
	  connection.sendVector(args);

	connection.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "free object" messages to FTS.*/

  final void freeObject(FtsObject obj)
  {
    try
      {
	unregisterObject(obj);
	connection.sendCmd(FtsClientProtocol.fts_free_object_cmd);
	connection.sendObject(obj);
	connection.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send an "object message" messages to FTS.*/

  final void sendObjectMessage(FtsObject dst, int inlet, String selector, Vector args)
  {
    try
      {
	connection.sendCmd(FtsClientProtocol.fts_message_cmd);
	connection.sendObject(dst);
	connection.sendInt(inlet);
	connection.sendString(selector);

	if (args != null)
	  connection.sendVector(args);

	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_named_message_cmd);

	connection.sendString(dst);
	connection.sendInt(inlet);
	connection.sendString(selector);

	if (args != null)
	  connection.sendVector(args);

	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_connect_objects_cmd);
	connection.sendObject(from);
	connection.sendInt(outlet);
	connection.sendObject(to);
	connection.sendInt(inlet);
	connection.sendEom();

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
	connection.sendCmd(FtsClientProtocol.fts_disconnect_objects_cmd);
	connection.sendObject(from);
	connection.sendInt(outlet);
	connection.sendObject(to);
	connection.sendInt(inlet);
	connection.sendEom();

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
	connection.sendCmd(FtsClientProtocol.fts_put_property_cmd);
	connection.sendObject(object);
	connection.sendString(name);
	connection.sendValue(value);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.fts_get_property_cmd);
	connection.sendObject(object);
	connection.sendString(name);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.ucs_cmd);
	connection.sendVector(args);
	connection.sendEom();
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
	connection.sendCmd(FtsClientProtocol.sync_cmd);
	connection.sendEom();
      }
    catch (java.io.IOException e)
      {
      }

    waitForPong();
  }


  /** Send a generic message to FTS. May be this is not used.*/

  final void sendMessage(FtsMessage msg)
  {
    connection.sendMessage(msg);
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

	  obj = (FtsObject) msg.getArguments().elementAt(0);
	  obj.localPut((String) msg.getArguments().elementAt(1), 
		       msg.getArguments().elementAt(2));
	}
      break;

      case FtsClientProtocol.fts_server_message_cmd:
	{
	  FtsObject obj;

	  obj = (FtsObject) msg.getArguments().elementAt(0);
	  obj.handleMessage(msg);
	}
      break;

      case FtsClientProtocol.post_cmd:
	System.out.print(msg.getArguments().elementAt(0)); // print the first argument
	break;

      case FtsClientProtocol.sync_done_cmd:
	deliverPong();
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

	wait(5000);
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
