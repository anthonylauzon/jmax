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

  FtsObject root;

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

  public FtsObject getRootObject()
  {
    return root;
  }

  /** Start the server. */

  public void start()
  {
    connection.start();

    // Build the root object and the root patcher

    root = FtsObject.makeRootObject(this);
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

  synchronized void openPatcher(FtsObject patcher)
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

  synchronized void closePatcher(FtsObject patcher)
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

  synchronized void patcherLoaded(FtsObject patcher)
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


  /** Send a "new object" messages to FTS, extracting the arguments and class name from the object*/

  synchronized void newObject(FtsObject patcher, FtsObject obj, Vector args)
  {
    try
      {
	registerObject(obj);
	connection.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	connection.sendObject(patcher);
	connection.sendInt(obj.getObjId());// cannot send the object, do not exists (yet) on the FTS Side !!
	connection.sendString(obj.getFtsClassName());


	if (args != null)
	  connection.sendVector(args);

	connection.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "redefine object" messages to FTS.*/

  synchronized void redefineObject(FtsObject obj, String className, Vector args)
  {
    try
      {
	connection.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	connection.sendObject(obj);
	connection.sendString(className);

	if (args != null)
	  connection.sendVector(args);

	connection.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "free object" messages to FTS.*/

  synchronized void freeObject(FtsObject obj)
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

  synchronized void sendObjectMessage(FtsObject dst, int inlet, String selector, Vector args)
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

  public synchronized void sendNamedObjectMessage(String dst, int inlet, String selector, Vector args)
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

  synchronized void connectObjects(FtsObject from, int outlet, FtsObject to, int inlet)
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

  synchronized void disconnectObjects(FtsObject from, int outlet, FtsObject to, int inlet)
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

  synchronized void putObjectProperty(FtsObject object, String name, Object value)
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

  synchronized void getObjectProperty(FtsObject object, String name)
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

  public synchronized void ucsMessage(Vector args)
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

  public synchronized void syncToFts()
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

  synchronized void sendMessage(FtsMessage msg)
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
	  obj.serverSetProperty((String) msg.getArguments().elementAt(1), 
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
