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
import java.net.*;
import java.io.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * Represent a running FTS server.
 * The server is represented by a structure of three object:
 *
 * <ul>
 * <li> the Fts class, that provide a public API to the application.
 * <li> the FtsServer class, that provide a  API to communicate with the 
 * server to the fts package.
 * <li> the FtsStream interface, and all its implementations, that provides
 * the physical communication channel with the actual server.
 * </ul>
 *
 * This class provide a way to start, stop, configure and  control the server, load and save files on it,
 * to instantiate objects and
 * connections on it, to redefine objects, 
 * handle selection and the server clipboard, to instantiate remote data instances and
 * register new remote data classes.
 * It also provide basic operations access and start an editor asyncronius on the 
 * Data property of an object, 
 * 
 */

public class Fts implements MaxContext
{
  public final static int FTS_NO_ID = -1; 
 /** The FTS server */
  FtsServer server = null;

  /** Get the FtsServer object for this server */

  public FtsServer getServer()
  {
    return server;
  }

  /** Build a new FTS server.
   * Instantiate a new server, by building the proper FtsStream object
   * and a new FtsServer instance; start the server  and initialize
   * it.
   * @param ftsDir the directory where to find the fts executable
   * @param ftsName the name of the fts executable
   * @param connectionType the type of connection to be used; currently the accepted values
   * are:
   * <dl>
   * <dt> <code>udp</code>
   * <dd> for an udp connection.
   * <dt> <code>tcp</code>
   * <dd> for a tcp connection.
   * <dt> <code>udpclient</code>
   * <dd> for a UDP based connection to an already open fts process
   * <dt> <code>socket</code> 
   * <dd> for a TCP based connection to an already open fts process
   * started in tcp server mode
   * <dt> <code>local</code>
   * <dd> for a pipe based connection; don't use this, it works, but
   * with enourmous latencies; we don't know why.
   * </dl>
   * @param host the name of the host where to run the server.
   * should be a complete internet domain name, or "local" to means the local
   * host.
   * @param port the number of port to be used, only for <code>socket</code> and 
   * <code>udpclient</code> connections.
   */

  public Fts(String ftsDir, String ftsName, String connectionType, String host, String ftsOptions, int port)
  {
    String project = MaxApplication.getProperty("project");
    if ( project != null)
      {
	ftsOptions = ftsOptions + " --project=" + project;
      }

    if (connectionType.equals("socket")) 
      server = new FtsServer(this, host, new FtsSocketStream( host, port));
    else if (connectionType.equals("udp")) 
      server = new FtsServer(this, host, new FtsDatagramStream( host, ftsDir, ftsName, ftsOptions));
    else if (connectionType.equals("udpclient")) 
      server = new FtsServer(this, host+":"+port, new FtsDatagramClientStream( port));
    else if (connectionType.equals("client") || connectionType.equals("tcp"))
      server = new FtsServer(this, host, new FtsSocketServerStream( host, ftsDir, ftsName, ftsOptions));
    else if (connectionType.equals("tcpclient"))
      server = new FtsServer(this, host, new FtsSocketClientStream( port));
    else if (connectionType.equals("local"))
      server = new FtsServer(this, "fts", new FtsSubProcessStream( ftsDir, ftsName, ftsOptions));
    else
      System.out.println("unknown FTS connection type "+ connectionType +": can't connect to FTS");

    server.start();
  }

  /** Create an FTS object. makeFtsObject do not actually create an
     object, but just assign an ID and ask FTS to create it; fts will
     create back the object with a message with the good ID, and we
     get the object thru the ID; if the object is not there, we got an
     instantiation error, and we throw a exception.  Here, we just use
     a className/description pair of arguments.  Version to use for
     those objects where className and description are available
     separately.  */

  public FtsObject makeFtsObject(FtsObject parent, String description) throws FtsException
  {
    FtsObject obj;
    int id;

    id = server.getNewObjectId();

    server.newObject(parent, id, description);
    server.sendDownloadObject(id);

    // Wait for FTS to do his work
    sync();
    
    obj = server.getObjectByFtsId(id);

    if (obj != null)
	{
	    obj.setDirty();
	    return obj;
	}
    else
	throw new FtsException("Instantiation error : " + description);
  }
  
  /** Make a Connection.
    It ask FTS to create the connection, and then upload it
    */

    public FtsConnection makeFtsConnection(FtsObject from, int outlet, FtsObject to, int inlet)
	throws FtsException
    {
	FtsConnection conn;
	int id;
      
	id = server.getNewConnectionId();
	
	server.newConnection(id, from, outlet, to, inlet);
	server.sendDownloadConnection(id);

	// Wait for FTS to do his work
	sync();

	conn = server.getConnectionByFtsId(id);
	
	if (conn != null)
	    {
		if (from.getParent() != null)
		    from.getParent().setDirty();

		return conn;
	    }
	else
	    throw new FtsException("Connection error");
      }


  /**
   * Redefine an FtsObject.
   *
   *  In case of errors, i.e. if the new object do not exists, we just throw an exception
   *  and do nothing.
   *
   * @param obj the object to redefine.
   * @param description  a string containing the description.
   * @return a new object, conforming to the new definition, but with the same FTS
   * identity, and connections; it always create a new object, also if the description
   * is the same.
   */

  public  FtsObject redefineFtsObject(FtsObject oldObject, String description) throws FtsException
  {
    Object listener;
    FtsObject newObject;
    FtsObject parent;
    int oldInlets, oldOutlets;
    Object data;

    // Get the listener, to move it to the new object

    listener = oldObject.getObjectListener();

    // Get the data, and quit the editors connected to the data

    oldObject.releaseData();

    // Remove the object from the selection if there

    getSelection().removeObject(oldObject);

    // Get parent and ins/outs

    parent = oldObject.getParent();

    oldInlets = oldObject.getNumberOfInlets();
    oldOutlets = oldObject.getNumberOfOutlets();

    // Allocate new Ids also for the redefinition.
    // make things simple, and anyway it is transparent.

    int newId;

    newId = server.getNewObjectId();

    // The object will be delete locally as an effect of the redefine
    // FTS will upload the object at the right moment.

    server.redefineObject(oldObject, newId, description);
    
    // Wait for FTS to do his work

    sync();
    newObject = server.getObjectByFtsId(newId);

    if (newObject != null)
      {
	newObject.setDirty();
	newObject.setObjectListener(listener);
      }
    else
      throw new FtsException("Instantiation Error: " + description);

    return newObject;
  }


  /** Selection handling */

  /** Get the Selection.
   * Get the unique FtsSelect object for this fts server.
   */

  private  FtsSelection selection = null;

  public final FtsSelection getSelection()
  {
    
    if (selection == null)
      {
	try
	  {
	    selection = (FtsSelection) makeFtsObject(server.getRootObject(), "__selection");
	  }
	catch (FtsException e)
	  {
	    System.out.println("System error: cannot get selection object");
	  }
      }
    
    return selection;
  }


  /** Syncronize with FTS.
   * Implement a syncronization point with FTS; when returning
   * this method, we are guaranteed that all the asyncronius requests sent 
   * to the server have been server, and all the uploades completed.
   */

  public void sync()
  {
    server.syncToFts();
  }

  /** Stop the server.
   * Shutdown the server; currently the server cannot be restarted,
   * so after this action this instance should not be referred anymore.
   */

  public void stop()
  {
    server.stop();
  }

  /** Get the root object for this server. */

  public  FtsObject getRootObject()
  {
    return server.getRootObject();
  }

  /** Get the Dsp Controller for this server. */ 
  FtsDspControl dspController = null;

  public FtsDspControl getDspController()
  {
      if (dspController == null)
      {
	try
	  {
	      dspController  = (FtsDspControl) makeFtsObject(server.getRootObject(), "__dspcontrol");
	  }
	catch (FtsException e)
	  {
	      System.out.println("System error: cannot get dsp control object");
	  }
      } 
  
    return dspController;
  }

  /** Get the Finder Object for this server. */ 
  FtsFinderObject finder = null;

  public FtsFinderObject getFinder()
  {
      if (finder == null)
      {
	try
	  {
	      finder  = (FtsFinderObject) makeFtsObject(server.getRootObject(), "__finder");
	  }
	catch (FtsException e)
	  {
	      System.out.println("System error: cannot get dsp control object");
	  }
      } 
  
    return finder;
  } 

  /** Get the Finder Object for this server. */ 
  FtsErrorFinderObject errorFinder = null;

  public FtsErrorFinderObject getErrorFinder()
  {
      if (errorFinder == null)
      {
	try
	  {
	      errorFinder = (FtsErrorFinderObject) makeFtsObject(server.getRootObject(), "__errorfinder");
	  }
	catch (FtsException e)
	  {
	      System.out.println("System error: cannot get dsp control object");
	  }
      } 
  
    return errorFinder;
  } 

  /*
   * The Delete object listeners; for the moment, only the object delete callback is really
   * fired.
   */

  MaxVector editListeners = new MaxVector();

  /** Install an Edit listener on this server */

  public  void addEditListener(FtsEditListener listener)
  {
    editListeners.addElement(listener);
  }

  /** Remove an Edit listener on this server */

  public  void removeEditListener(FtsEditListener listener)
  {
    editListeners.removeElement(listener);
  }

  public void fireObjectRemoved(FtsObject object)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).objectRemoved(object);
  }
  public void fireObjectAdded(FtsObject object)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).objectAdded(object);
  }
  public void fireConnectionAdded(FtsConnection connection)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).connectionAdded(connection);
  }
  public void fireAtomicAction(boolean active)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).atomicAction(active);
  }
  /** Load a binary jmax file. @return its root patcher. */

  public FtsObject loadJMaxFile(File file)
  {
    int id;

    id = server.getNewObjectId();
    server.loadPatcherBmax(server.getRootObject(), id, file.getAbsolutePath());
    server.sendDownloadObject(id);
    server.syncToFts();    
    return server.getObjectByFtsId(id);
  }

  /** Save the patcher in the given file. */

  public void saveJMaxFile(FtsObject patcher, File file)
  {
    server.savePatcherBmax(patcher, file.getAbsolutePath());
  }

  /* Load a .pat Max 0.26 file. @return its root patcher */

  public FtsObject loadPatFile(File file)
  {
    int id;

    // Build an empty patcher son of root.

    id = server.getNewObjectId();

    // ask fts to load the file within this 
    // patcher, using a dedicated message

    server.loadPatcherDpat(server.getRootObject(), id, file.getAbsolutePath());
    server.sendDownloadObject(id);
    server.syncToFts();
    return server.getObjectByFtsId(id);
  }

  // Support for configuration
  

  /** Declare an Max 0.26 abstraction */

  public void abstractionDeclare(String name, String filename)
  {
    server.sendAbstractionDeclare(name, filename);
  }


  /** Declare an Max 0.26 abstraction  path */

  public void abstractionPathDeclare(String path)
  {
    server.sendAbstractionDeclarePath(path);
  }


  /** Declare a template*/

  public void templateDeclare(String name, String filename)
  {
    server.sendTemplateDeclare(name, filename);
    server.sendRecomputeErrorObjects();
  }


  /** Declare a template path*/

  public void templatePathDeclare(String path)
  {
    server.sendTemplateDeclarePath(path);
    server.sendRecomputeErrorObjects();
  }


  /** Execute on the server an UCS command */

  public void ucsCommand(MaxVector args)
  {
    server.ucsMessage(args);
  }

  // Update group listener

  /** Add an update group listener */

  public void addUpdateGroupListener(FtsUpdateGroupListener listener)
  {
    server.addUpdateGroupListener(listener);
  }


  /** Remove an update group listener */

  public void removeUpdateGroupListener(FtsUpdateGroupListener listener)
  {
    server.removeUpdateGroupListener(listener);
  }
}





