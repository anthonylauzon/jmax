//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
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

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;


/**
 * This class export a number of global functionalities
 * implemented by the application layer, as static methods.
 */

public class Fts
{
  /** The FTS server */

  static FtsServer server = null;

  static public FtsServer getServer()
  {
    return server;
  }
  
  public static void connectToFts(String ftsDir, String ftsName, String mode,
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
      server = new FtsServer(serverName, new FtsSocketStream(serverName, Integer.parseInt(port)));
    else if (mode.equals("udp")) 
      server = new FtsServer(serverName, new FtsDatagramStream(serverName, ftsDir, ftsName));
    // else if (mode.equals("udprx")) 
    // server = new FtsServer(serverName, new FtsRexecDatagramStream(serverName, ftsDir, ftsName));
    else if (mode.equals("udpclient")) 
      server = new FtsServer(serverName + ":" + port,
			     new FtsDatagramClientStream(serverName, ftsDir, ftsName, Integer.parseInt(port)));
    else if (mode.equals("client") || mode.equals("tcp"))
      server = new FtsServer(serverName, new FtsSocketServerStream(serverName, ftsDir, ftsName));
    else if (mode.equals("local"))
      server = new FtsServer("fts", new FtsSubProcessStream(ftsDir, ftsName));
    else
      System.out.println("unknown FTS connection type "+mode+": can't connect to FTS");

    server.start();
  }

  /** makeFtsObject do not create an object, but just assign an
     ID and ask FTS to create it; fts will create back the object with a message
     with the good ID, and we get the object thru the ID; if the object
     is not there, we got an instantiation error, and we throw a exception.
     Here, we just use a className/description pair of arguments.
     Version to use for those objects where className and description
     are available separately.
     */

  static public FtsObject makeFtsObject(FtsObject parent, String className, String description)
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

  static public FtsObject makeFtsObject(FtsObject parent, String description) throws FtsException
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

  static public FtsConnection makeFtsConnection(FtsObject from, int outlet, FtsObject to, int inlet)
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
   * Static function to redefine a FtsObject.
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

  public static FtsObject redefineFtsObject(FtsObject oldObject, String description) throws FtsException
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

    Fts.getSelection().removeObject(oldObject);

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

  public static FtsRemoteData newRemoteData(String name, Object args[])
  {
    return FtsRemoteMetaData.getRemoteMetaData().newInstance(name, args);
  }

  /** Selection handling */

  /** Selection
   * Get the unique FtsSelect object for this container
   */

  private static FtsSelection selection = null;

  static public final FtsSelection getSelection()
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


  /* handling of user/password */

  private static String userName;
  private static String userPassword;

  static public void setUserName(String name)
  {
    userName = name;
  }

  static public void setUserPassword(String password)
  {
    userPassword = password;
  }

  static String getUserName()
  {
    return userName;
  }

  static String getUserPassword()
  {
    return userPassword;
  }


  // Handling of non real-time mode
  private static boolean noRealTime = false;

  public static void setNoRealTime( boolean noRealTime)
  {
    Fts.noRealTime = noRealTime;
  }

  public static boolean getNoRealTime()
  {
    return noRealTime;
  }


  /* Sync command */

  static public void sync()
  {
    getServer().syncToFts();
  }

  /* The RemoteMetaData class data base */

  static private Hashtable remoteDataClassTable = new Hashtable();

  static public void registerRemoteDataClass(String name, Class dataClass)
  {
    remoteDataClassTable.put(name, dataClass);
  }

  static Class getRemoteDataClass(String name)
  {
    return (Class) remoteDataClassTable.get(name);
  }

  /* Get the root object */

  public static FtsObject getRootObject()
  {
    return server.getRootObject();
  }

  /* Ask FTS to recompute error objects.
     Error objects actually need to be recomputed only 
     after changing the environment */

  public static void recomputeErrorObjects()
  {
    server.sendRecomputeErrorObjects();
  }

  /* New Data listener support: listeners are installed on an object;
     only one listener for object.
   */

  static private Hashtable newDataListeners = new Hashtable();


  static public void addNewDataListenerOn(FtsNewDataListener listener, FtsObject obj)
  {
    newDataListeners.put(obj, listener);
  }

  static public void removeNewDataListenerOn(FtsObject obj)
  {
    newDataListeners.remove(obj);
  }

  static void fireNewDataListenerOn(FtsObject obj, MaxData data)
  {
    FtsNewDataListener listener;

    listener = (FtsNewDataListener) newDataListeners.get(obj);

    if (listener != null)
      listener.newDataFor(obj, data);
  }

  /* Utility function: get a Data that is a value of an object property,
     and call and editor on it; but do it asynchroniously;
     the asynchronicity is actually in two places:
     1- to avoid calling Fts.sync, put a property handler on the wished property.

     2- Once the property handler will be called by the input thread, a Runnable
	starting the editor will be posted using invokeLater, so it will be
	executed in the AWT thread.
    */

  static class DelayedEditPropertyHandler implements FtsNewDataListener
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

      Fts.removeNewDataListenerOn(obj);
    }
  }

  public static void editPropertyValue(FtsObject obj, MaxDataEditorReadyListener listener)
  {
    Fts.addNewDataListenerOn(new DelayedEditPropertyHandler(listener), obj);
    obj.updateData();
  }


  public static void editPropertyValue(FtsObject obj, Object where,
				       MaxDataEditorReadyListener listener)
  {
    Fts.addNewDataListenerOn(new DelayedEditPropertyHandler(listener, where), obj);
    obj.updateData();
  }

  /* Get the Fts Dsp Controller */ 

  static FtsDspControl dspController = null;

  static public FtsDspControl getDspController()
  {
    if (dspController == null)
      dspController = (FtsDspControl) Fts.newRemoteData("dspcontrol_data", null);
    
    return dspController;
  }

  /*
   * The Delete object listeners; for the moment, only the object delete callback is really
   * fired.
   */

  static MaxVector editListeners = new MaxVector();

  public static void addEditListener(FtsEditListener listener)
  {
    editListeners.addElement(listener);
  }

  public static void removeEditListener(FtsEditListener listener)
  {
    editListeners.removeElement(listener);
  }

  public static void fireObjectRemoved(FtsObject object)
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

  static public MaxDocumentType  getPatcherDocumentType()
  {
    return patcherType;
  }
}
