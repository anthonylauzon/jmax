package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;


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
  public static boolean debug = false;

  boolean connected = false;
  boolean waiting = false;

  /** The FtsPort used to communicate with FTS */

  FtsPort port;

  /** The name of this server, used for printouts only */

  String name;

  /** The root object of this server */

  FtsContainerObject root;

  /** The list of listener of update groups */
  
  MaxVector updateGroupListeners;

  /** If true, put a 10 sec timeout on Sync;
   */

  private boolean timeoutOnSync = false;

  /** Create an FTS Server. With a given port. */

  FtsServer(String name, FtsPort port)
  {
    this.name = name;
    this.port = port;

    this.port.setServer(this);

    connected = true;
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

    // Build the root patcher, by mapping directly to object id 1 on FTS
    // (this is guaranteed)

    root = new FtsPatcherObject(null, "root 0 0", 1);
    registerObject(root);
  }

  /** Stop the server. */

  public void stop()
  {
    port.close();

    /* This will cause the actual close to happen */

    syncToFts();
  }

  /** Set a server parameter, <i>before</i> the server start. */

  public void setParameter(String name, String value)
  {
    port.setParameter(name, value);
  }


  /** Send a "save patcher as bmax" messages to FTS.*/

  final void savePatcherBmax(FtsObject patcher, String filename)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("savePatcherBmax(" + patcher + "," + filename + ")");

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

  /** send a "load patcher as bmax" messages to FTS.*/

  final void loadPatcherBmax(FtsObject parent, int id, String filename)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("loadPatcherBmax(" + parent + ", " + id + ", " + filename + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_load_patcher_bmax_cmd);
	port.sendObject(parent);
	port.sendInt(id);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** send a "load patcher as dot pat" messages to FTS.*/

  final void loadPatcherDpat(FtsObject parent, int id, String filename)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("loadPatcherDpat(" + parent + ", " + id + ", " + filename + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_load_patcher_dpat_cmd);
	port.sendObject(parent);
	port.sendInt(id);
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
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendAbstractionDeclare(" + abstraction + ", " + filename + ")");

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
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendAbstractionDeclarePath(" + path + ")");

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


  /** Send a "template declare" message to FTS */

  final public void sendTemplateDeclare(String template, String filename)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendTemplateDeclare(" + template + ", " + filename + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_declare_template_cmd);
	port.sendString(template);
	port.sendString(filename);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "template declare" message to FTS */

  final public void sendTemplateDeclarePath(String path)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendTemplateDeclarePath(" + path + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_declare_template_path_cmd);
	port.sendString(path);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "download patcher" messages to FTS and do a sync */

  final void sendDownloadPatcher(FtsObject patcher)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendDownloadPatcher(" + patcher + ")");

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


  /** Send a "download patcher" messages to FTS.*/

  final void sendDownloadPatcher(int id)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendDownloadPatcher(" + id + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_download_patcher_cmd);
	port.sendObject(id);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "open patcher" messages to FTS.*/

  final void openPatcher(FtsObject patcher)
  {
    if (! connected)
      return;


    if (FtsServer.debug)
      System.err.println("openPatcher(" + patcher + ")");

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
    if (! connected)
      return;

    if (FtsServer.debug) 
      System.err.println("closePatcher(" + patcher + ")");

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


  /** Send a "new object" messages to FTS; receive the class Name and a description as a string;
   *  It is actually used only for those objects that don't have the class name in the description,
   *  i.e. messages.
   */

  final  void newObject(FtsObject patcher, int id, String className, String description)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("newObject(" + patcher + ", " + id + ", " + className + ", " + description + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(id);
	port.sendString(className);
	FtsParse.parseAndSendObject(description, port);
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
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("newObject(" + patcher + ", " + id + ", " + description + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	port.sendObject(patcher);
	port.sendInt(id);// cannot send the object, do not exists (yet) on the FTS Side !!

	FtsParse.parseAndSendObject(description, port);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "download object" messages to FTS;
   */

  final  void sendDownloadObject(int id)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendDownloadObject(" + id + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_download_object_cmd);
	port.sendObject(id);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "redefine patcher" messages to a patcher in FTS.
   *  Special optimized version for patcher loading/editing
   */

  final void redefinePatcherObject(FtsObject obj, String description)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("redefinePatcherObject(" + description + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_redefine_patcher_cmd);
	port.sendObject(obj);
	FtsParse.parseAndSendObject(description, port);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send a "redefine object" messages to an object in FTS.
   */

  final void redefineObject(FtsObject obj, int newId, String description)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("redefineObject(" + description + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	port.sendObject(obj);
	port.sendInt(newId);
	FtsParse.parseAndSendObject(description, port);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Reposition an inlet  */

  final void repositionInletObject(FtsObject obj, int pos)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("repositionInletObject(" + obj + ", " + pos + ")");

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
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("repositionOutletObject(" + obj + ", " + pos + ")");

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

  final void deleteObject(FtsObject obj)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("deleteObject(" + obj + ")");

    if (obj.getObjectId() != -1)
      {
	try
	  {
	    port.sendCmd(FtsClientProtocol.fts_delete_object_cmd);
	    port.sendObject(obj);
	    port.sendEom();
	  }
	catch (java.io.IOException e)
	  {
	  }
      }
  }

  /** Send an "object message" messages to FTS.*/

  final void sendObjectMessage(FtsObject dst, int inlet, String selector, MaxVector args)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendObjectMessage(" + dst + ", " + inlet + ", " + selector + ", " + args + ")");

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

  /** Send an "object message" with a unique FtsObject argument  to FTS.*/

  final void sendObjectMessage(FtsObject dst, int inlet, String selector, FtsObject arg)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendObjectMessage(" + dst + ", " + inlet + ", " + selector + ", " + arg + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_message_cmd);
	port.sendObject(dst);
	port.sendInt(inlet);
	port.sendString(selector);
	port.sendObject(arg);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Send an "object message" with a unique FtsConnection argument  to FTS.*/

  final void sendObjectMessage(FtsObject dst, int inlet, String selector, FtsConnection arg)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendObjectMessage(" + dst + ", " + inlet + ", " + selector + ", " + arg + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_message_cmd);
	port.sendObject(dst);
	port.sendInt(inlet);
	port.sendString(selector);
	port.sendConnection(arg);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send an "set" messages to the system inletof an FTS object with as arguments elements
    from a Vector; the vector should only contains String, Floats and Integers, of course.
    */

  final void sendSetMessage(FtsObject dst, MaxVector values)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendSetMessage(" + dst + ", " + values  + ")");

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
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendSetMessage(" + obj + ", " + description + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_message_cmd);
	port.sendObject(obj);
	port.sendInt(-1);
	port.sendString("set");

	FtsParse.parseAndSendObject(description, port);

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "named object message" messages to FTS.
    @deprecated
   */

  final public void sendNamedObjectMessage(String dst, int inlet, String selector, MaxVector args)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendNamedObjectMessage(" + dst + ", " + inlet + ", " +
			 selector + ", " + args + ")");

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

  final void newConnection(int id, FtsObject from, int outlet, FtsObject to, int inlet)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("newConnection(" + id + "," + from + ", " + outlet + ", " +
			 to + ", " + inlet + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_new_connection_cmd);
	port.sendInt(id);
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

  /** Send a "download connection" messages to FTS;
   */

  final  void sendDownloadConnection(int id)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("sendDownloadConnection(" + id + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_download_connection_cmd);
	port.sendConnection(id);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "disconnect objects" messages to FTS. */

  final void deleteConnection(FtsConnection connection)
  {
    if (! connected)
      return;

    if (FtsServer.debug) 
      System.err.println("deleteConnection(" + connection + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_delete_connection_cmd);
	port.sendConnection(connection);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "put property" messages to FTS. */

  final void putObjectProperty(FtsObject object, String name, Object value)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("putObjectProperty(" + object + ", " + name + ", " +
			 value + ")");

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

  final void askObjectProperty(FtsObject object, String name)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("askObjectProperty(" + object + ", " + name + ")");

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

  final public void ucsMessage(MaxVector args)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("ucsMessage(" + args + ")");

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

  /** Send a "remote call" message to FTS. */

  final void remoteCall( FtsRemoteData data, int key, Object args[])
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println( "remoteCall(" + data + ", " + key + "," + args + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.remote_call_code);
	port.sendRemoteData(data);
	port.sendInt(key);

	if (args != null)
	  port.sendArray( args); // we may have zero args call

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, args)");
      }
  }

  final void remoteCall( FtsRemoteData data, int key, MaxVector args)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println( "remoteCall(" + data + ", " + key + "," + args + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.remote_call_code);
	port.sendRemoteData(data);
	port.sendInt(key);

	if (args != null)
	  port.sendVector( args); // we may have zero args call

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, args)");
      }
  }

  final void remoteCall( FtsRemoteData data, int key, int offset, int size, int values[])
  {
    if (! connected)
      return;

    if (FtsServer.debug) 
      System.err.println( "remoteCall(" + data +
			  ", " + key + ", " + offset + ", " + size + ", " + values 
			  + "[" + values.length + "]" + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.remote_call_code);
	port.sendRemoteData(data);
	port.sendInt(key);
	port.sendInt(offset);
	port.sendInt(size);
	port.sendArray(values, offset, size);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, offset, size , values)");
      }
  }

  final void remoteCall( FtsRemoteData data, int  key, int id, String name, Object args[])
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println( "remoteCall(" + data + ", " + key + ", " + id + ", " + name + ", " + args + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.remote_call_code);
	port.sendRemoteData(data);
	port.sendInt(key);
	port.sendInt(id);
	port.sendString(name);

	if (args != null)
	  port.sendArray( args); // we may have zero args call

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, id, name, args)");
      }
  }

  final void remoteCall( FtsRemoteData data, int  key, FtsObject object, Object args[])
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println( "remoteCall(" + data + ", " + key + ", " + object + ", " + args + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.remote_call_code);
	port.sendRemoteData(data);
	port.sendInt(key);
	port.sendObject(object);

	if (args != null)
	  port.sendArray( args); // we may have zero args call

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, object, args)");
      }
  }


  final void remoteCall( FtsRemoteData data, int  key, FtsObject object, MaxVector args)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println( "remoteCall(" + data + ", " + key + ", " + object + ", " + args + ")");

    try
      {
	port.sendCmd(FtsClientProtocol.remote_call_code);
	port.sendRemoteData(data);
	port.sendInt(key);
	port.sendObject(object);

	if (args != null)
	  port.sendVector( args); // we may have zero args call

	port.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, object, args)");
      }
  }

  final void sendRecomputeErrorObjects()
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("recomputeErrorObjects()");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_recompute_error_objects_cmd);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer.recomputeErrorObjects()");
      }
  }

  final void sendShutdown()
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("shutdown");

    try
      {
	port.sendCmd(FtsClientProtocol.fts_shutdown_cmd);
	port.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer.shutdown()");
      }
  }


  /**
   * Sync point with FTS.
   * Send a ping message, and sychroniuosly
   * wait for FTS to answer; this guarantee that all the
   * pending callbacks and updates from FTS have been processed,
   * and all the values are consistents with the FTS counterpart.
   * Note that now a eom always imply a flush, so no explicit flush
   * are needed anymore in sync.
   */

  final public synchronized void syncToFts()
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("syncToFts()");

    try
      {
	port.sendCmd(FtsClientProtocol.sync_cmd);
	port.sendEom();
      }

    catch (java.io.IOException e)
      {
      }

    waitForPong();
  }


  /** Handle adding an update group listener */

  public void addUpdateGroupListener(FtsUpdateGroupListener listener)
  {
    if (updateGroupListeners == null)
      updateGroupListeners = new MaxVector();
    
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
   * 
   * Note that given asynchronius operations, an object or data can be delete
   * while having still messages (usually properties) arriving; for this
   * reason, messages with null  destinations are simply ignored
   */

  void dispatchMessage(FtsMessage msg)
  {
    if (FtsServer.debug)
      System.err.println("Received message " + msg);

    switch (msg.getCommand())
      {
      case FtsClientProtocol.fts_property_value_cmd:
	{
	  FtsObject obj;
	  String prop;
	  Object value;
	  
	  obj = (FtsObject) msg.getNextArgument();
	  prop = ((String) msg.getNextArgument()).intern();
	  value =  msg.getNextArgument();

	  if ((obj == null) || (prop == null) || (value == null))
 	    System.err.println("Wrong property value message " + msg);
	  else
	    {
 	      if (obj != null)
 		obj.localPut(prop, value);
	      
	      if (FtsServer.debug)
 		System.err.println("SetPropertyValue " + obj + " " + prop + " " + value);
 	    }
	}
      break;

      case FtsClientProtocol.fts_server_message_cmd:
	{
	  FtsObject obj;

	  obj = (FtsObject) msg.getNextArgument();

	  if (obj != null)
	    obj.handleMessage(msg);

	  if (FtsServer.debug)
	    System.err.println("ObjectMessage " + obj + " " + msg);
	}
      break;

      case FtsClientProtocol.post_cmd:
	System.out.print(msg.getNextArgument()); // print the first argument
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

      case FtsClientProtocol.fts_new_connection_cmd:
	{
	  int id;
	  FtsObject from, to;
	  int outlet, inlet;
	  FtsConnection c;

	  id     = ((Integer)  msg.getNextArgument()).intValue();
	  from   = (FtsObject) msg.getNextArgument();
	  outlet = ((Integer)  msg.getNextArgument()).intValue();
	  to     = (FtsObject) msg.getNextArgument();
	  inlet  = ((Integer)  msg.getNextArgument()).intValue();

	  c = new FtsConnection(id, from, outlet, to, inlet);

	  registerConnection(c);

	  if (FtsServer.debug)
	    System.err.println("New Connection #" + id + " " + from + "." + outlet + " -> " + to + "." + inlet);
	}
      break;

      case FtsClientProtocol.fts_redefine_connection_cmd:
	{
	  int id;
	  FtsObject from, to;
	  int outlet, inlet;
	  FtsConnection c;

	  c      = (FtsConnection)  msg.getNextArgument();
	  from   = (FtsObject) msg.getNextArgument();
	  outlet = ((Integer)  msg.getNextArgument()).intValue();
	  to     = (FtsObject) msg.getNextArgument();
	  inlet  = ((Integer)  msg.getNextArgument()).intValue();

	  c.redefine(from, outlet, to, inlet);

	  if (FtsServer.debug)
	    System.err.println("Connection Redefined " + c);
	}
      break;

      case FtsClientProtocol.fts_release_connection_cmd:
	{
	  FtsConnection c;

	  c = (FtsConnection)  msg.getNextArgument();

	  if (FtsServer.debug) 
	    System.err.println("Connection Release" + c);


	  Fts.getSelection().removeConnection(c);
	  c.release();
	  break;
	}

      case FtsClientProtocol.fts_release_object_cmd:
	{
	  FtsObject obj;

	  obj = (FtsObject)  msg.getNextArgument();

	  if (FtsServer.debug) 
	    System.err.println("Object Release" + obj);

	  obj.release();
	  break;
	}

      case FtsClientProtocol.fts_release_object_data_cmd:
	{
	  FtsObject obj;

	  obj = (FtsObject)  msg.getNextArgument();

	  if (FtsServer.debug) 
	    System.err.println("Object Release Data" + obj);

	  obj.releaseData();
	  break;
	}

      case FtsClientProtocol.remote_call_code:
	{
	  FtsRemoteData data = (FtsRemoteData) msg.getNextArgument();
	  Integer value = (Integer) msg.getNextArgument();

	  if ((data != null) && (value != null))
	    {
	      int key = value.intValue();
	      
	      data.call(key, msg);
	    }
	  else
	    System.err.println( "Wrong remote call message " + msg);
	  break;
	}

      default:
	System.err.println( "!!!!!!!!! Received unknown message from client " + msg);
	break;
      }
  }

  /** Synchronization primitive for the Ping/Pong protocol. */

  private synchronized void waitForPong()
  {
    // If FTS quitted, just return 

    if (connected)
      {
	try
	  {
	    // naive solution to the FTS crash problem
	    // it should be smarter, detect the problem,
	    // and set the server as halted.

	    waiting = true;

	    if (timeoutOnSync)
	      wait(10000);
	    else
	      wait();

	    waiting = false;
	  }
	catch (java.lang.InterruptedException e)
	  {
	    // ignore iand continue
	  }
      }
  }

  /** Synchronization primitive for the Ping/Pong protocol. */

  private synchronized void deliverPong()
  {
    notifyAll();
  }

  /** Recovery for FTS Crashes */

  void ftsQuitted()
  {
    connected = false;

    if (waiting)
      deliverPong(); // Should we test if we are wa
  }

  /* OBJECT ID HANDLING */

  /**
   * The server Object ID Counter.
   * 
   * NOTE: IDs must start from 3:
   *      0 is reserved as null object
   *      1 is reserved for the root object
   *      even numbers are used by the server
   * 
   *
   * Used to generate IDs for FTS objects; skip zero and one
   * and continue by increments
   */

  private int ftsObjectIDCounter = 3;	// skip zero and one

  /** The object table. Used to map back Ids to objects. */

  private MaxVector objectTable = new MaxVector();

  /** The max object Id registered */

  private int maxObjectId = 0;

  /**
   * Object ID handling.
   * Each server instance have it own object table, and provide
   * a register and unregister function; also, the server store and access
   * the object id directly in the FtsObject, for efficency, but this is 
   * transparent to the object.
   *
   * The Client use only odd object IDs; server generated objects use
   * only even IDs; on the server, IDs are assigned by need,
   * not to all the objects.
   */

  void registerObject(FtsObject object)
  {
    if (object == null)
      System.err.println("registerObject got a null object");

    if (object.getObjectId() == -1)
      {
	int newid;

	newid = ftsObjectIDCounter;
	ftsObjectIDCounter += 2;

	object.setObjectId(newid);
      }

    if (object.getObjectId() > maxObjectId)
      {
	maxObjectId = object.getObjectId() * 2;

	objectTable.setSize(maxObjectId + 1);
      }

    objectTable.setElementAt(object, object.getObjectId());
  }

  /**
   * This method reserve (and return) a new id for an object, and make
   * place for it in the object table, but just put a null there.
   *
   */

  int getNewObjectId()
  {
    int newid;

    newid = ftsObjectIDCounter;
    ftsObjectIDCounter += 2;

    if (newid > maxObjectId)
      {
	maxObjectId = newid * 2;

	objectTable.setSize(maxObjectId + 1);
      }

    objectTable.setElementAt(null, newid);

    return newid;
  }

  /**
   * ID handling.
   * Each server instance have it own object table, and provide
   * a register and unregister function; also, the server store and access
   * the object id directly in the FtsObject, for efficency, but this is 
   * transparent to the object.
   */

  void unregisterObject(FtsObject obj)
  {
    objectTable.setElementAt(null, obj.getObjectId());
  }

  /**
   * Get an FtsObject by its FtsId.
   *
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
	Thread.dumpStack();
	return null;
      }
    else
      {
	FtsObject object = (FtsObject) objectTable.elementAt(id);

	// Returned obj can be null here !!

	return object;
      }
  }

  /* CONNECTION ID HANDLING */

  /**
   * The server Connection ID Counter.
   *
   * Used to generate IDs for FTS connections; skip zero, and one
   * and continue by two increments
   */

  private int ftsConnectionIDCounter = 3;	// skip zero and one

  /** The connection table. Used to map back Ids to connections. */

  private MaxVector connectionTable = new MaxVector();

  /** The max connection Id registered */

  private int maxConnectionId = 0;

  /**
   * Connection ID handling.
   * Each server instance have it own connection table, and provide
   * a register and unregister function; also, the server store and access
   * the connection id directly in the FtsConnection, for efficency, but this is 
   * transparent to the connection.
   *
   * The Client use only odd connection IDs; server generated connections use
   * only even IDs; on the server, IDs are assigned by need,
   * not to all the connections.
   */

  private void registerConnection(FtsConnection connection)
  {
    if (connection == null)
      System.err.println("registerConnection got a null connection");

    if (connection.getConnectionId() == -1)
      {
	int newid;

	newid = ftsConnectionIDCounter;
	ftsConnectionIDCounter += 2;

	connection.setConnectionId(newid);
      }

    if (connection.getConnectionId() > maxConnectionId)
      {
	maxConnectionId = connection.getConnectionId() * 2;

	connectionTable.setSize(maxConnectionId + 1);
      }

    connectionTable.setElementAt(connection, connection.getConnectionId());
  }

  /**
   * This method reserve (and return) a new id for an connection, and make
   * place for it in the connection table, but just put a null there.
   *
   */

  int getNewConnectionId()
  {
    int newid;

    newid = ftsConnectionIDCounter;
    ftsConnectionIDCounter += 2;

    if (newid > maxConnectionId)
      {
	maxConnectionId = newid * 2;

	connectionTable.setSize(maxConnectionId + 1);
      }

    connectionTable.setElementAt(null, newid);

    return newid;
  }

  /**
   * ID handling.
   * Each server instance have it own connection table, and provide
   * a register and unregister function; also, the server store and access
   * the connection id directly in the FtsConnection, for efficency, but this is 
   * transparent to the connection.
   */

  private void unregisterConnection(FtsConnection obj)
  {
    connectionTable.setElementAt(null, obj.getConnectionId());
  }

  /**
   * Get an FtsConnection by its FtsId.
   *
   */

  FtsConnection getConnectionByFtsId(int id)
  {
    if (id == 0)
      {
	// This is usually the case at the creation of the root patcher

	return null;
      }
    else if (id == -1)
      {
	System.err.println("System error: received FTS_NO_ID as connection id\n");
	Thread.dumpStack();
	return null;
      }
    else
      {
	FtsConnection connection = (FtsConnection) connectionTable.elementAt(id);

	// Returned obj can be null here !!

	return connection;
      }
  }
}







