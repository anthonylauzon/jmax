package ircam.jmax.fts;

import ircam.jmax.*;

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

  static boolean debug = false;

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

  /** The FtsPort used to communicate with FTS */

  FtsPort port;

  /** The name of this server, used for printouts only */

  String name;

  /** The root object of this server */

  FtsContainerObject root;

  /** The list of listener of update groups */
  
  Vector updateGroupListeners;

  /** If true, put a 10 sec timeout on Sync;
    You can avoid the timeout with the -noTimeOut yes
    command line flag
   */

  private boolean timeoutOnSync = true;

  /** Create an FTS Server. With a given port. */

  FtsServer(String name, FtsPort port)
  {
    String timeOut;

    this.name = name;
    this.port = port;

    timeOut = MaxApplication.getProperty("noTimeOut");

    if (timeOut != null)
      timeoutOnSync = false;

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

    // root = FtsPatcherObject.makeRootObject(this);

    try
      {
	root = (FtsContainerObject) FtsObject.makeFtsObject(null, "patcher", "root 0 0");
      }
    catch (FtsException e)
      {
	// Deep shit 
	System.out.println("System Error: Unable to create the root object");
      }
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

  /** Send a "save patcher as bmax" messages to FTS.*/

  final void savePatcherBmax(FtsObject patcher, String filename)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_save_patcher_bmax_cmd);
	port.sendObject(patcher);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "save patcher as tpat" messages to FTS.*/

  final void savePatcherTpat(FtsObject patcher, String filename)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_save_patcher_tpat_cmd);
	port.sendObject(patcher);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** send a "load patcher as bmax" messages to FTS.*/

  final void loadPatcherBmax(FtsObject patcher, String filename)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_load_patcher_bmax_cmd);
	port.sendObject(patcher);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** send a "load patcher as tpat" messages to FTS.*/

  final void loadPatcherTpat(FtsObject patcher, String filename)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_load_patcher_tpat_cmd);
	port.sendObject(patcher);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** send a "load patcher as dot pat" messages to FTS.*/

  final void loadPatcherDpat(FtsObject patcher, String filename)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_load_patcher_dpat_cmd);
	port.sendObject(patcher);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "abstraction declare" message to FTS */

  final public void sendAbstractionDeclare(String abstraction, String filename)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_declare_abstraction_cmd);
	port.sendString(abstraction);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "abstraction declare" message to FTS */

  final public void sendAbstractionDeclarePath(String path)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_declare_abstraction_path_cmd);
	port.sendString(path);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "download patcher" messages to FTS.*/

  final void sendDownloadPatcher(FtsObject patcher)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_download_patcher_cmd);
	port.sendObject(patcher);
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

  final  void newObject(FtsObject patcher, int id, String className, String description)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(id);
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

  final  void newObject(FtsObject patcher, int id, String description)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(id);// cannot send the object, do not exists (yet) on the FTS Side !!

	FtsParseToPort.parseAndSendObject(description, port);

	port.sendEom();
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
	port.sendCmd(FtsClientProtocol.fts_redefine_patcher_cmd);
	port.sendObject(obj);
	port.sendString(name);
	port.sendInt(ninlets);
	port.sendInt(noutlets);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Reposition an inlet  */

  final void repositionInletObject(FtsObject obj, int pos)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_reposition_inlet_cmd);
	port.sendObject(obj);
	port.sendInt(pos);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Reposition an outlet  */

  final void repositionOutletObject(FtsObject obj, int pos)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_reposition_outlet_cmd);
	port.sendObject(obj);
	port.sendInt(pos);
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


  /** Send set" messages to the system inlet 0 of an FTS object with as arguments elements
    from an int arrays
    */

  final void sendSetMessage(FtsObject dst, int offset, int[] values, int from, int to)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_message_cmd);
	port.sendObject(dst);
	port.sendInt(-1);
	port.sendString("set");
	port.sendInt(offset);

	if (values != null)
	  port.sendArray(values, from, to);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send an "set" messages to the system inletof an FTS object with as arguments elements
    from a Vector; the vector should only contains String, Floats and Integers, of course.
    */

  final void sendSetMessage(FtsObject dst, Vector values)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_message_cmd);
	port.sendObject(dst);
	port.sendInt(-1);
	port.sendString("set");

	if (values != null)
	  port.sendVector(values);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send an "set" messages to the system inletof an FTS object with as arguments elements
    from a string description.
    */

  final void sendSetMessage(FtsObject obj, String description)
  {
    try
      {
	port.sendCmd(FtsClientProtocol.fts_message_cmd);
	port.sendObject(obj);
	port.sendInt(-1);
	port.sendString("set");

	FtsParseToPort.parseAndSendObject(description, port);

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
	  if (msg.getNumberOfArguments() >= 3)
	    {
	      FtsObject obj;

	      obj = (FtsObject) msg.getArgument(0);

	      if (obj == null)
		System.err.println("Null object in property value message " + msg);
	      else
		obj.localPut((String) msg.getArgument(1), msg.getArgument(2));

	      if (FtsServer.debug)
		System.err.println("SetPropertyValue " + obj + " " + msg.getArgument(1)
				   + " " + msg.getArgument(2));
	    }
	  else
	    System.err.println("Wrong property value message " + msg);
	}
      break;

      case FtsClientProtocol.fts_server_message_cmd:
	{
	  FtsObject obj;

	  obj = (FtsObject) msg.getArgument(0);
	  obj.handleMessage(msg);

	  if (FtsServer.debug)
	    System.err.println("ObjectMessage " + obj + " " + msg);
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

	// Messages add to support creation on demand of FtsObject 
	// on server request.
	// Object deletetion will (for now, at least, always happen
	// on client request

      case FtsClientProtocol.fts_new_object_cmd:
	{
	  FtsObject newObj;

	  try
	    {
	      newObj = FtsObject.makeFtsObjectFromMessage(msg);
	      registerObject(newObj);

	      if (FtsServer.debug)
		System.err.println("NewObjectMessage " + newObj + " " + msg);
	    }
	  catch (FtsException e)
	    {
	      System.err.println("System error, cannot instantiate from message " + msg);
	    }

	  break;
	}

      case FtsClientProtocol.fts_new_abstraction_cmd:
	{
	  FtsObject newObj;

	  try
	    {
	      newObj = FtsObject.makeFtsAbstractionFromMessage(msg);
	      registerObject(newObj);

	      if (FtsServer.debug)
		System.err.println("NewAbstractionMessage " + newObj + " " + msg);
	    }
	  catch (FtsException e)
	    {
	      System.err.println("System error, cannot instantiate from message " + msg);
	    }

	  break;
	}

      case FtsClientProtocol.fts_connect_objects_cmd:
	{
	  FtsObject from, to;
	  int outlet, inlet;

	  from = (FtsObject) msg.getArgument(0);
	  outlet = ((Integer) msg.getArgument(1)).intValue();
	  to = (FtsObject) msg.getArgument(2);
	  inlet = ((Integer) msg.getArgument(3)).intValue();

	  new FtsConnection(from, outlet, to, inlet, false);

	  if (FtsServer.debug)
	    System.err.println("NewConnection " + from + "." + outlet + " -> " + to + "." + inlet);
	}
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

	if (timeoutOnSync)
	  wait(10000);
	else
	  wait();
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
   *
   * The Client use only odd IDs; server generated objects use
   * only even IDs; on the server, IDs are assigned by need,
   * not to all the objects.
   */

  private void registerObject(FtsObject obj)
  {
    if (obj == null)
      System.err.println("registerObject got a null object");

    if (obj.getObjId() == -1)
      {
	int newid;

	newid = ftsIDCounter;
	ftsIDCounter += 2;

	obj.setObjId(newid);
      }

    if (obj.getObjId() > maxId)
      {
	maxId = obj.getObjId() * 2;

	objTable.setSize(maxId + 1);
      }

    objTable.setElementAt(obj, obj.getObjId());
  }

  /**
   * This method reserve (and return) a new id for an object, and make
   * place for it in the object table, but just put a null there.
   *
   */

  int getNewObjectId()
  {
    int newid;

    newid = ftsIDCounter;
    ftsIDCounter += 2;

    if (newid > maxId)
      {
	maxId = newid * 2;

	objTable.setSize(maxId + 1);
      }

    objTable.setElementAt(null, newid);

    return newid;
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
  }

  /**
   * Get an FtsObject by its FtsId (used only by the FAL).
   *
   * @deprecated
   */

  FtsObject getObjectByFtsId(int id)
  {
    
    if (id == 0)
      {
	// This is usually the case at the creation of the root patcher

	return null;
      }
    else if (id == -1)
      {
	System.err.println("System error: received FTS_NO_ID as object id\n");
	return null;
      }
    else
      {
	FtsObject obj = (FtsObject) objTable.elementAt(id);

	// Returned obj can be null here !!

	return obj;
      }
  }
}







