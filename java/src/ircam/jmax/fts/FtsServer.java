//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

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


public class FtsServer  implements Runnable
{
  Fts fts;			// back pointer to the FTS object using this server

  public static boolean debug = false;

  boolean connected = false;
  boolean waiting = false;

  /** The FtsStream used to communicate with FTS */

  FtsStream stream;

  /** The name of this server, used for printouts only */

  String name;

  /** The root object of this server */

  FtsObject root;

  /** The list of listener of update groups */
  
  MaxVector updateGroupListeners;
  int updateGroupDepth = 0;

  /** If true, put a 10 sec timeout on Sync;
   */

  private boolean timeoutOnSync = false;

  /** Create an FTS Server. With a given stream. */

  FtsServer(Fts fts, String name, FtsStream stream)
  {
    this.fts = fts;
    this.name = name;
    this.stream = stream;

    this.stream.setServer(this);

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

  public FtsObject getRootObject()
  {
    return root;
  }

  public Fts getFts()
  {
    return fts;
  }

  //
  // Server Housekeeping
  // 


  //
  // Outgoing messages
  // 


  /** Send a "save patcher as bmax" messages to FTS.*/

  final void savePatcherBmax(FtsObject patcher, String filename)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("> savePatcherBmax(" + patcher + "," + filename + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_save_patcher_bmax_cmd);
	stream.sendObject(patcher);
	stream.sendString(filename);
	stream.sendEom();
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
      System.err.println("> loadPatcherBmax(" + parent + ", " + id + ", " + filename + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_load_patcher_bmax_cmd);
	stream.sendObject(parent);
	stream.sendInt(id);
	stream.sendString(filename);
	stream.sendEom();
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
      System.err.println("> loadPatcherDpat(" + parent + ", " + id + ", " + filename + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_load_patcher_dpat_cmd);
	stream.sendObject(parent);
	stream.sendInt(id);
	stream.sendString(filename);
	stream.sendEom();
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
      System.err.println("> sendAbstractionDeclare(" + abstraction + ", " + filename + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_declare_abstraction_cmd);
	stream.sendString(abstraction);
	stream.sendString(filename);
	stream.sendEom();
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
      System.err.println("> sendAbstractionDeclarePath(" + path + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_declare_abstraction_path_cmd);
	stream.sendString(path);
	stream.sendEom();
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
      System.err.println("> sendTemplateDeclare(" + template + ", " + filename + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_declare_template_cmd);
	stream.sendString(template);
	stream.sendString(filename);
	stream.sendEom();
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
      System.err.println("> sendTemplateDeclarePath(" + path + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_declare_template_path_cmd);
	stream.sendString(path);
	stream.sendEom();
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
      System.err.println("> newObject(" + patcher + ", " + id + ", " + className + ", " + description + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	stream.sendObject(patcher);
	stream.sendInt(id);
	stream.sendString(className);
	FtsParse.parseAndSendObject(description, stream);
	stream.sendEom();
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
      System.err.println("> newObject(" + patcher + ", " + id + ", " + description + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_new_object_cmd);
	stream.sendObject(patcher);
	stream.sendInt(id);// cannot send the object, do not exists (yet) on the FTS Side !!

	FtsParse.parseAndSendObject(description, stream);

	stream.sendEom();
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
      System.err.println("> sendDownloadObject(" + id + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_download_object_cmd);
	stream.sendObject(id);
	stream.sendEom();
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
      System.err.println("> redefinePatcherObject(" + description + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_redefine_patcher_cmd);
	stream.sendObject(obj);
	FtsParse.parseAndSendObject(description, stream);
	stream.sendEom();
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
      System.err.println("> redefineObject(" + description + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_redefine_object_cmd);
	stream.sendObject(obj);
	stream.sendInt(newId);
	FtsParse.parseAndSendObject(description, stream);
	stream.sendEom();
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
      System.err.println("> repositionInletObject(" + obj + ", " + pos + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_reposition_inlet_cmd);
	stream.sendObject(obj);
	stream.sendInt(pos);
	stream.sendEom();
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
      System.err.println("> repositionOutletObject(" + obj + ", " + pos + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_reposition_outlet_cmd);
	stream.sendObject(obj);
	stream.sendInt(pos);
	stream.sendEom();
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
      System.err.println("> deleteObject(" + obj + ")");

    if (obj.getObjectId() != -1)
      {
	try
	  {
	    stream.sendCmd(FtsClientProtocol.fts_delete_object_cmd);
	    stream.sendObject(obj);
	    stream.sendEom();
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
      System.err.println("> sendObjectMessage(" + dst + ", " + inlet + ", " + selector + ", " + args + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_message_cmd);
	stream.sendObject(dst);
	stream.sendInt(inlet);
	stream.sendString(selector);

	if (args != null)
	  stream.sendVector(args);

	stream.sendEom();
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
      System.err.println("> sendObjectMessage(" + dst + ", " + inlet + ", " + selector + ", " + arg + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_message_cmd);
	stream.sendObject(dst);
	stream.sendInt(inlet);
	stream.sendString(selector);
	stream.sendObject(arg);
	stream.sendEom();
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
      System.err.println("> sendObjectMessage(" + dst + ", " + inlet + ", " + selector + ", " + arg + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_message_cmd);
	stream.sendObject(dst);
	stream.sendInt(inlet);
	stream.sendString(selector);
	stream.sendConnection(arg);
	stream.sendEom();
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
      System.err.println("> sendSetMessage(" + dst + ", " + values  + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_message_cmd);
	stream.sendObject(dst);
	stream.sendInt(-1);
	stream.sendString("set");

	if (values != null)
	  stream.sendVector(values);

	stream.sendEom();
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
      System.err.println("> sendSetMessage(" + obj + ", " + description + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_message_cmd);
	stream.sendObject(obj);
	stream.sendInt(-1);
	stream.sendString("set");

	FtsParse.parseAndSendObject(description, stream);

	stream.sendEom();
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
      System.err.println("> newConnection(" + id + "," + from + ", " + outlet + ", " +
			 to + ", " + inlet + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_new_connection_cmd);
	stream.sendInt(id);
	stream.sendObject(from);
	stream.sendInt(outlet);
	stream.sendObject(to);
	stream.sendInt(inlet);
	stream.sendEom();
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
      System.err.println("> sendDownloadConnection(" + id + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_download_connection_cmd);
	stream.sendConnection(id);
	stream.sendEom();
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
      System.err.println("> deleteConnection(" + connection + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_delete_connection_cmd);
	stream.sendConnection(connection);
	stream.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "put property" messages to FTS; version with a generic value */

  final void putObjectProperty(FtsObject object, String name, Object value)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("> putObjectProperty(" + object + ", " + name + ", " +
			 value + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_put_property_cmd);
	stream.sendObject(object);
	stream.sendString(name);
	stream.sendValue(value);
	stream.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "put property" messages to FTS; version with an int value */

  final void putObjectProperty(FtsObject object, String name, int value)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("> putObjectProperty(" + object + ", " + name + ", " +
			 value + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_put_property_cmd);
	stream.sendObject(object);
	stream.sendString(name);
	stream.sendInt(value);
	stream.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }


  /** Send a "put property" messages to FTS; version with a float*/

  final void putObjectProperty(FtsObject object, String name, float value)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println("> putObjectProperty(" + object + ", " + name + ", " +
			 value + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_put_property_cmd);
	stream.sendObject(object);
	stream.sendString(name);
	stream.sendFloat(value);
	stream.sendEom();
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
      System.err.println("> askObjectProperty(" + object + ", " + name + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_get_property_cmd);
	stream.sendObject(object);
	stream.sendString(name);
	stream.sendEom();
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
      System.err.println("> ucsMessage(" + args + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.ucs_cmd);
	stream.sendVector(args);
	stream.sendEom();
      }
    catch (java.io.IOException e)
      {
      }
  }

  /** Start a remote call */

  final void remoteCallStart(FtsRemoteData data, int key)
  {
    if (! connected)
      return;

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }

  final void remoteCallAddArg(int arg)
  {
    if (! connected)
      return;

    try
      {
	stream.sendInt(arg);
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }

  final void remoteCallAddArg(Integer arg)
  {
    if (! connected)
      return;

    try
      {
	stream.sendInt(arg);
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }

  final void remoteCallAddArg(float arg)
  {
    if (! connected)
      return;

    try
      {
	stream.sendFloat(arg);
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }

  final void remoteCallAddArg(Float arg)
  {
    if (! connected)
      return;

    try
      {
	stream.sendFloat(arg);
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }

  final void remoteCallAddArg(FtsObject arg)
  {
    if (! connected)
      return;

    try
      {
	stream.sendObject(arg);
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }

  final void remoteCallAddArg(String arg)
  {
    if (! connected)
      return;

    try
      {
	stream.sendString(arg);
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }

  final void remoteCallAddArg(FtsRemoteData arg)
  {
    if (! connected)
      return;

    try
      {
	stream.sendRemoteData(arg);
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }


  final void remoteCallEnd()
  {
    if (! connected)
      return;

    try
      {
	stream.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCallStart()");
      }
  }


  /** Send a single argument "remote call" message to FTS. */

  final void remoteCall( FtsRemoteData data, int key, int arg)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println( "> remoteCall(" + data + ", " + key + "," + arg + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);
	stream.sendInt(arg);
	stream.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, args)");
      }
  }

  final void remoteCall( FtsRemoteData data, int key, float arg)
  {
    if (! connected)
      return;

    if (FtsServer.debug)
      System.err.println( "> remoteCall(" + data + ", " + key + "," + arg + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);
	stream.sendFloat(arg);
	stream.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, args)");
      }
  }

  final void remoteCall( FtsRemoteData data, int key, Object arg)
  {
    if (! connected)
      return;

    if (FtsServer.debug) 
      System.err.println( "> remoteCall(" + data + ", " + key + "," + arg + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);
	stream.sendValue(arg);
	stream.sendEom();
      }
    catch (java.io.IOException e)
      {
	System.err.println("IOException in FtsServer:remoteCall(data, key, args)");
      }
  }

  /** Send a "remote call" message to FTS. */

  final void remoteCall( FtsRemoteData data, int key, Object args[])
  {
    if (! connected)
      return;

    if (FtsServer.debug) 
      System.err.println( "> remoteCall(" + data + ", " + key + "," + args + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);

	if (args != null)
	  stream.sendArray( args); // we may have zero args call

	stream.sendEom();
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
      System.err.println( "> remoteCall(" + data + ", " + key + "," + args + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);

	if (args != null)
	  stream.sendVector( args); // we may have zero args call

	stream.sendEom();
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
      System.err.println( "> remoteCall(" + data +
			  ", " + key + ", " + offset + ", " + size + ", " + values 
			  + "[" + values.length + "]" + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);
	stream.sendInt(offset);
	stream.sendInt(size);
	stream.sendArray(values, offset, size);
	stream.sendEom();
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
      System.err.println( "> remoteCall(" + data + ", " + key + ", " + id + ", " + name + ", " + args + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);
	stream.sendInt(id);
	stream.sendString(name);

	if (args != null)
	  stream.sendArray( args); // we may have zero args call

	stream.sendEom();
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
      System.err.println( "> remoteCall(" + data + ", " + key + ", " + object + ", " + args + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);
	stream.sendObject(object);

	if (args != null)
	  stream.sendArray( args); // we may have zero args call

	stream.sendEom();
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
      System.err.println( "> remoteCall(" + data + ", " + key + ", " + object + ", " + args + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.remote_call);
	stream.sendRemoteData(data);
	stream.sendInt(key);
	stream.sendObject(object);

	if (args != null)
	  stream.sendVector( args); // we may have zero args call

	stream.sendEom();
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
      System.err.println("> recomputeErrorObjects()");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_recompute_error_objects_cmd);
	stream.sendEom();
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
      System.err.println("> shutdown");

    try
      {
	stream.sendCmd(FtsClientProtocol.fts_shutdown_cmd);
	stream.sendEom();
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
   *
   * Return true if the sync was ok, return false if there was a server
   * timeout
   */

  final public synchronized boolean syncToFts()
  {
    boolean ret;

    if (! connected)
      return false;

    if (FtsServer.debug)
      System.err.println("> syncToFts()");

    try
      {
	stream.sendCmd(FtsClientProtocol.sync_cmd);
	stream.sendEom();
      }

    catch (java.io.IOException e)
      {
      }

    if (timeoutOnSync)
      ret = waitForPong(10000);
    else
      ret = waitForPong(0);

    if (FtsServer.debug)
      System.err.println("> sync Done");

    return ret;
  }

  /* Version with a timeout, to check if FTS is still alive 
   * Return true if the sync was ok, return false if there was a server
   * timeout
   */

  final public synchronized boolean syncToFts(int timeOut)
  {
    boolean ret;

    if (! connected)
      return false;

    if (FtsServer.debug)
      System.err.println("> syncToFts(" + timeOut + ")");

    try
      {
	stream.sendCmd(FtsClientProtocol.sync_cmd);
	stream.sendEom();
      }

    catch (java.io.IOException e)
      {
      }

    ret = waitForPong(timeOut);

    if (FtsServer.debug)
      System.err.println("> sync Done");

    return ret;
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


  //
  // Incoming message handling
  //

  private Thread inputThread;

  /** Start the server. */

  public void start()
  {
    inputThread = new Thread(this, name);
    inputThread.setPriority(Thread.MAX_PRIORITY);
    inputThread.start(); 

    //if (! syncToFts(10000))
    // System.err.println("Connection to FTS failed !!!");


    // Build the root patcher, by mapping directly to object id 1 on FTS
    // (this is guaranteed)

    root = new FtsPatcherObject(fts, null, null, "", 1);
    registerObject(root);
  }

  /** Stop the server. */

  public void stop()
  {
    sendShutdown();
  }

  /** the main loop of the input listener thread. */

  public void run()
  {
    boolean running  = true;

    while (running && stream.isOpen())
      {
	try
	  {
	    dispatchMessage(stream);
	  }
	catch (java.io.InterruptedIOException e)
	  {
	    /* Ignore, just retry */
	  }
	catch (FtsQuittedException e)
	  {
	    ftsQuitted();
	    running = false;
	  }
	catch (java.lang.Exception e)
	  {
	    // Try to survive an exception
	    
	    // System.err.println("System exception " + e);
	    // e.printStackTrace();
	  }
      }

    // close the thread and the streams

    stream.close();
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

  void dispatchMessage(FtsStream stream)
       throws java.io.InterruptedIOException, FtsQuittedException, java.io.IOException
  {
    switch (stream.getCommand())
      {
      case FtsClientProtocol.fts_property_value_cmd:
	{
	  //
	  // Note that a null value can be acceptable for
	  // some properties 
	  //

	  FtsObject obj;
	  String prop;
	  Object value;
	  
	  obj = stream.getNextObjectArgument();
	  prop = stream.getNextSymbolArgument();

	  if (FtsServer.debug)
	    {
	      if (stream.nextIsInt())
		{
		  int v = stream.getNextIntArgument();

		  System.err.println("< SetPropertyValue  " + prop + " to " + v + " for object " + obj);
		  obj.localPut(prop, v);
		}
	      else if (stream.nextIsFloat())
		{
		  float v = stream.getNextFloatArgument();

		  System.err.println("< SetPropertyValue  " + prop + " to " + v + " for object " + obj);
		  obj.localPut(prop, v);
		}
	      else
		{
		  Object v = stream.getNextArgument();

		  System.err.println("< SetPropertyValue  " + prop + " to " + v + " for object " + obj);
		  obj.localPut(prop, v);
		}
	    }
	  else
	    {
	      if (stream.nextIsInt())
		obj.localPut(prop, stream.getNextIntArgument());
	      if (stream.nextIsFloat())
		obj.localPut(prop, stream.getNextFloatArgument());
	      else
		obj.localPut(prop, stream.getNextArgument());
	    }
	      
	}
      break;

      case FtsClientProtocol.fts_server_message_cmd:
	{
	  FtsObject obj;

	  obj = stream.getNextObjectArgument();

	  obj.handleMessage(stream);

	  if (FtsServer.debug)
	    System.err.println("< ObjectMessage for " + obj + " in " + stream);
	}
      break;

      case FtsClientProtocol.post_cmd:
	System.out.print(stream.getNextStringArgument()); // print the first argument
	break;

      case FtsClientProtocol.post_line_cmd:
	// WHAT TO DO IF OUTPUT IS NOT DIRECTED TO CONSOLE ?
	// append first argument (a complete line) to the console
	ircam.jmax.editors.console.ConsoleWindow.append(stream.getNextStringArgument());
	break;

      case FtsClientProtocol.sync_done_cmd:


	if (FtsServer.debug)
	  System.err.println("< Pong Message");

	deliverPong();
	break;
	
      case FtsClientProtocol.fts_update_group_start_cmd:

	if (FtsServer.debug)
	  System.err.println("< Update Group Start");

	if (updateGroupDepth == 0)
	  {
	    updateGroupDepth++;	
	
	    if (updateGroupListeners != null)
	      for (int i = 0; i < updateGroupListeners.size(); i++)
		((FtsUpdateGroupListener) updateGroupListeners.elementAt(i)).updateGroupStart();
	  }
	else
	  updateGroupDepth++;	
	
	break;

      case FtsClientProtocol.fts_update_group_end_cmd:
	if (FtsServer.debug)
	  System.err.println("< Update Group End");

	updateGroupDepth--;

	if (updateGroupDepth == 0)
	  {
	    if (updateGroupListeners != null)
	      for (int i = 0; i < updateGroupListeners.size(); i++)
		((FtsUpdateGroupListener) updateGroupListeners.elementAt(i)).updateGroupEnd();
	  }
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
	      newObj = FtsObject.makeFtsObjectFromMessage(fts, stream, false);
	      registerObject(newObj);

	      if (FtsServer.debug)
		System.err.println("< NewObjectMessage " + newObj + " in " + stream);
	    }
	  catch (FtsException e)
	    {
	      System.err.println("System error, cannot instantiate from stream " + stream);
	    }

	  break;
	}

      case FtsClientProtocol.fts_new_object_var_cmd:
	{
	  FtsObject newObj;

	  try
	    {
	      newObj = FtsObject.makeFtsObjectFromMessage(fts, stream, true);
	      registerObject(newObj);

	      if (FtsServer.debug)
		System.err.println("< NewObjectMessage " + newObj + " in " + stream);
	    }
	  catch (FtsException e)
	    {
	      System.err.println("System error, cannot instantiate from stream " + stream);
	    }

	  break;
	}

      case FtsClientProtocol.fts_new_connection_cmd:
	{
	  FtsPatcherData data;
	  int id;
	  FtsObject from, to;
	  int outlet, inlet;
	  FtsConnection c;

	  data   = (FtsPatcherData) stream.getNextDataArgument();
	  id     = stream.getNextIntArgument();
	  from   = stream.getNextObjectArgument();
	  outlet = stream.getNextIntArgument();
	  to     = stream.getNextObjectArgument();
	  inlet  = stream.getNextIntArgument();

	  c = new FtsConnection(fts, data, id, from, outlet, to, inlet);

	  registerConnection(c);

	  if (FtsServer.debug)
	    System.err.println("< New Connection #" + id + " " + from + "." + outlet + " -> " + to + "." + inlet);
	}
      break;

      case FtsClientProtocol.fts_redefine_connection_cmd:
	{
	  int id;
	  FtsObject from, to;
	  int outlet, inlet;
	  FtsConnection c;

	  c      = stream.getNextConnectionArgument();
	  from   = stream.getNextObjectArgument();
	  outlet = stream.getNextIntArgument();
	  to     = stream.getNextObjectArgument();
	  inlet  = stream.getNextIntArgument();

	  c.redefine(from, outlet, to, inlet);

	  if (FtsServer.debug)
	    System.err.println("< Connection Redefined " + c);
	}
      break;

      case FtsClientProtocol.fts_release_connection_cmd:
	{
	  FtsConnection c;

	  c = stream.getNextConnectionArgument();

	  if (FtsServer.debug) 
	    System.err.println("< Connection Release " + c);

	  fts.getSelection().removeConnection(c);
	  c.release();
	  break;
	}

      case FtsClientProtocol.fts_release_object_cmd:
	{
	  FtsObject obj;

	  obj = stream.getNextObjectArgument();

	  if (FtsServer.debug) 
	    System.err.println("< Object Release" + obj);

	  obj.release();
	  break;
	}

      case FtsClientProtocol.fts_release_object_data_cmd:
	{
	  FtsObject obj;

	  obj = stream.getNextObjectArgument();

	  if (FtsServer.debug) 
	    System.err.println("< Object Release Data" + obj);

	  obj.releaseData();
	  break;
	}

      case FtsClientProtocol.remote_call:
	{
	  FtsRemoteData data = stream.getNextDataArgument();
	  int key = stream.getNextIntArgument();

	  if (FtsServer.debug) 
	    System.err.println("< remote Call stream: " + data + " key " + key + " stream "  + stream);

	  data.call(key, stream);

	  break;
	}

      default:
	System.err.println( "!!!!!!!!! Received unknown message from stream " + stream);
	break;
      }
  }

  /** Synchronization primitive for the Ping/Pong protocol. */

  private synchronized boolean waitForPong(int timeOut)
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

	    if (timeOut != 0)
	      wait(timeOut);
	    else
	      wait();

	    if (waiting)
	      {
		// Timeout exit
		connected = false;
		return false;
	      }
	    else
	      return true;
	  }
	catch (java.lang.InterruptedException e)
	  {
	    // ignore iand continue

	    return true;
	  }
      }
    else
      return false;
  }

  /** Synchronization primitive for the Ping/Pong protocol. */

  private synchronized void deliverPong()
  {
    waiting = false;
    notifyAll();
  }

  /** Recovery for FTS Crashes */

  void ftsQuitted()
  {
    connected = false;

    if (waiting)
      deliverPong(); // Should we test if we are wa
  }

  //
  /// OBJECT ID HANDLING 
  //

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

  // Handle the remote table ID

  FtsRemoteDataID remoteTable = new FtsRemoteDataID();

  FtsRemoteDataID getRemoteTable()
  {
    return remoteTable;
  }
}







