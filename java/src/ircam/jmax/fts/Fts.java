//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.fts;

import java.util.*;
import java.net.*;
import java.io.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;


/**
 * This class export a number of global functionalities
 * implemented by the application layer, as static methods.
 */

public class Fts implements MaxContext
{
  /** The FTS server */

  FtsServer server = null;

  FtsServer getServer()
  {
    return server;
  }
  
  public Fts(String ftsDir, String ftsName, String mode,
	     String serverName, String port)
  {
    if (serverName.equals("local"))
      {
	try
	  {
	    serverName = InetAddress.getLocalHost().getHostName();
	  } 
	catch (java.net.UnknownHostException e)
	  {
	    return;
	  }
      }

    if (MaxApplication.getProperty("ssrun") != null)
      ftsName = ftsName + ".ss";

    if (mode.equals("socket")) 
      server = new FtsServer(this, serverName, new FtsSocketStream(serverName, Integer.parseInt(port)));
    else if (mode.equals("udp")) 
      server = new FtsServer(this, serverName, new FtsDatagramStream(serverName, ftsDir, ftsName));
    else if (mode.equals("udpclient")) 
      server = new FtsServer(this, serverName + ":" + port,
			     new FtsDatagramClientStream(serverName, ftsDir, ftsName, Integer.parseInt(port)));
    else if (mode.equals("client") || mode.equals("tcp"))
      server = new FtsServer(this, serverName, new FtsSocketServerStream(serverName, ftsDir, ftsName));
    else if (mode.equals("local"))
      server = new FtsServer(this, "fts", new FtsSubProcessStream(ftsDir, ftsName));
    else
      System.out.println("unknown FTS connection type "+mode+": can't connect to FTS");

    server.start();

    // Install the meta data object

    FtsRemoteMetaData.install(this);

  }

  /** makeFtsObject do not create an object, but just assign an
     ID and ask FTS to create it; fts will create back the object with a message
     with the good ID, and we get the object thru the ID; if the object
     is not there, we got an instantiation error, and we throw a exception.
     Here, we just use a className/description pair of arguments.
     Version to use for those objects where className and description
     are available separately.
     */

  public FtsObject makeFtsObject(FtsObject parent, String className, String description)
       throws FtsException
  {
    FtsObject obj;
    int id;

    id = server.getNewObjectId();

    server.newObject(parent, id, className, description);
    server.sendDownloadObject(id);
    
    // Wait for FTS to do his work

    sync();

    obj = server.getObjectByFtsId(id);
    
    if (obj != null)
      {
	if (parent != null)
	  parent.setDirty();

	return obj;
      }
    else
      throw new FtsException("Instantiation error for " + className + " " + description);
  }
  
  /* 
     Version to use for those objects where className and description
     are available separately.
     */

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
  

  /** makeFtsConnection work as makeFtsObject, but for connections */

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
   * function to redefine a FtsObject.
   * It is static, for
   * similarity with the constructors, because it can produce 
   * a different Java object from the argument, so logically they are not method of the object.
   *
   *
   *  In case of errors, i.e. if the new object do not exists, we just throw an exception
   *  and do nothing.
   *
   * @param obj the object to redefine.
   * @param description  a string containing the description.
   * @return a new object, conforming to the new definition, but with the same FTS
   * identity, and connections; it always create a new object, also if the description
   * is the same
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

    data = oldObject.getData();	

    if (data instanceof MaxData)
      Mda.dispose((MaxData) data);

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


  /* Data objects */

  public  FtsRemoteData newRemoteData(String name, Object args[])
  {
    return FtsRemoteMetaData.getRemoteMetaData().newInstance(name, args);
  }

  /** Selection handling */

  /** Selection
   * Get the unique FtsSelect object for this container
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


  /* Sync command */

  public void sync()
  {
    server.syncToFts();
  }

  /* Stop command */

  public void stop()
  {
    server.stop();
  }

  /* The RemoteMetaData class data base */

  private static Hashtable remoteDataClassTable = new Hashtable();

  public static void registerRemoteDataClass(String name, Class dataClass)
  {
    remoteDataClassTable.put(name, dataClass);
  }

  static Class getRemoteDataClass(String name)
  {
    return (Class) remoteDataClassTable.get(name);
  }

  /* Get the root object */

  public  FtsObject getRootObject()
  {
    return server.getRootObject();
  }


  /* New Data listener support: listeners are installed on an object;
     only one listener for object.
   */

  private Hashtable newDataListeners = new Hashtable();


  public void addNewDataListenerOn(FtsNewDataListener listener, FtsObject obj)
  {
    newDataListeners.put(obj, listener);
  }

  public void removeNewDataListenerOn(FtsObject obj)
  {
    newDataListeners.remove(obj);
  }

  void fireNewDataListenerOn(FtsObject obj, MaxData data)
  {
    FtsNewDataListener listener;

    listener = (FtsNewDataListener) newDataListeners.get(obj);

    if (listener != null)
      listener.newDataFor(obj, data);
  }

  /* Utility function: get a Data that is a value of an object property,
     and call and editor on it; but do it asynchroniously;
     the asynchronicity is actually in two places:
     1- to avoid calling sync, put a property handler on the wished property.

     2- Once the property handler will be called by the input thread, a Runnable
	starting the editor will be posted using invokeLater, so it will be
	executed in the AWT thread.
    */

  class DelayedEditPropertyHandler implements FtsNewDataListener
  {
    MaxDataEditorReadyListener listener;
    Object where;

    DelayedEditPropertyHandler(MaxDataEditorReadyListener listener, Object where)
    {
      this.listener = listener;
      this.where    = where;
    }

    DelayedEditPropertyHandler(MaxDataEditorReadyListener listener)
    {
      this(listener, null);
    }

    public void newDataFor(FtsObject obj, MaxData newData)
    {
      final MaxData data = (MaxData) newData;

      if (data != null)
	{
	  // Set the document; when documents will be remote data,
	  // it will be handled in FTS.

	  if (data instanceof FtsRemoteData)
	    ((FtsRemoteData) data).setDocument(obj.getDocument());

	  SwingUtilities.invokeLater(new Runnable() {
	    public void run() {
	      MaxDataEditor editor;

	      try
		{
		  editor = Mda.edit(data, where);
		  editor.addEditorReadyListener(listener);
		}
	      catch (MaxDocumentException e)
		{
		  // Error; we should do an
		  listener.editorReady(null); 
		}
	    }});
	}
      else
	listener.editorReady(null);

      removeNewDataListenerOn(obj);
    }
  }

  public  void editPropertyValue(FtsObject obj, MaxDataEditorReadyListener listener)
  {
    addNewDataListenerOn(new DelayedEditPropertyHandler(listener), obj);
    obj.updateData();
  }


  public  void editPropertyValue(FtsObject obj, Object where,
				 MaxDataEditorReadyListener listener)
  {
    addNewDataListenerOn(new DelayedEditPropertyHandler(listener, where), obj);
    obj.updateData();
  }

  /* Get the Fts Dsp Controller */ 

  FtsDspControl dspController = null;

  public FtsDspControl getDspController()
  {
    if (dspController == null)
      dspController = (FtsDspControl) newRemoteData("dspcontrol_data", null);
    
    return dspController;
  }

  /*
   * The Delete object listeners; for the moment, only the object delete callback is really
   * fired.
   */

  MaxVector editListeners = new MaxVector();

  public  void addEditListener(FtsEditListener listener)
  {
    editListeners.addElement(listener);
  }

  public  void removeEditListener(FtsEditListener listener)
  {
    editListeners.removeElement(listener);
  }

  public  void fireObjectRemoved(FtsObject object)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).objectRemoved(object);
  }

  // MDA SUpport

  private static MaxDocumentType patcherType;

  static void setPatcherDocumentType(MaxDocumentType type)
  {
    patcherType = type;
  }

  public static MaxDocumentType  getPatcherDocumentType()
  {
    return patcherType;
  }

  /* Load a binary jmax file, and return its root patcher */

  public FtsObject loadJMaxFile(File file)
  {
    int id;

    id = server.getNewObjectId();
    server.loadPatcherBmax(server.getRootObject(), id, file.getAbsolutePath());
    server.sendDownloadObject(id);
    server.syncToFts();    

    return server.getObjectByFtsId(id);
  }

  /** Save the given patcher in the given file */

  public void saveJMaxFile(FtsObject patcher, File file)
  {
    server.savePatcherBmax(patcher, file.getAbsolutePath());
  }

  /* Load a .pat Max 0.26 file, and return its root patcher */

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
  
  public void abstractionDeclare(String name, String filename)
  {
    server.sendAbstractionDeclare(name, filename);
  }

  public void abstractionPathDeclare(String path)
  {
    server.sendAbstractionDeclarePath(path);
  }

  public void templateDeclare(String name, String filename)
  {
    server.sendTemplateDeclare(name, filename);
    server.sendRecomputeErrorObjects();
  }

  public void templatePathDeclare(String path)
  {
    server.sendTemplateDeclarePath(path);
    server.sendRecomputeErrorObjects();
  }

  public void ucsCommand(MaxVector args)
  {
    server.ucsMessage(args);
  }

  // Update group listener

  public void addUpdateGroupListener(FtsUpdateGroupListener listener)
  {
    server.addUpdateGroupListener(listener);
  }

  public void removeUpdateGroupListener(FtsUpdateGroupListener listener)
  {
    server.removeUpdateGroupListener(listener);
  }
}
