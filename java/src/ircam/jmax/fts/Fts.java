package ircam.jmax.fts;

import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;
import com.sun.java.swing.*;

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
  
  public static void connectToFts(String theFtsdir, String theFtsname, String mode,
				  String serverName, String port)
  {
    if (mode.equals("socket")) 
      server = new FtsSocketServer(serverName, Integer.parseInt(port));
    else if (mode.equals("udp")) 
      server = new FtsDatagramServer(serverName);
    else if (mode.equals("udprx")) 
      server = new FtsRexecDatagramServer(serverName);
    else if (mode.equals("udpclient")) 
      server = new FtsDatagramClientServer(serverName, Integer.parseInt(port));
    else if (mode.equals("client"))
      server = new FtsSocketClientServer(serverName);
    else if (mode.equals("local"))
      server = new FtsSubProcessServer();
    else
      System.out.println("unknown FTS connection type "+mode+": can't connect to FTS");

    server.setParameter("ftsdir", theFtsdir);
    server.setParameter("ftsname", theFtsname);

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

  static public FtsObject makeFtsObject(FtsContainerObject parent, String className, String description)
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
      throw new FtsException( new FtsError(FtsError.INSTANTIATION_ERROR, className + " " + description));
  }
  
  /* 
     Version to use for those objects where className and description
     are available separately.
     */

  static public FtsObject makeFtsObject(FtsContainerObject parent, String description) throws FtsException
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
      throw new FtsException(new FtsError(FtsError.INSTANTIATION_ERROR, description));
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
      throw new FtsException( new FtsError(FtsError.CONNECTION_ERROR, "Connection error"));
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
    FtsObject newObject;
    FtsContainerObject parent;
    int oldInlets, oldOutlets;
    Object data;

    // Get the data, and quit the editors connected to the data

    data = oldObject.get("data");	

    if (data instanceof MaxData)
      Mda.dispose((MaxData) data);

    // Get parent and ins/outs

    parent = oldObject.getParent();

    oldInlets = oldObject.getNumberOfInlets();
    oldOutlets = oldObject.getNumberOfOutlets();

    // makeFtsObject can throw a connection if the
    // object do not exists.

    int id;

    id = oldObject.getObjectId();

    // Delete locally the old object

    server.redefineObject(oldObject, description);
    oldObject.release();
    server.sendDownloadObject(id);
    
    // Wait for FTS to do his work

    sync();
    newObject = server.getObjectByFtsId(id);
    
    if (newObject != null)
      newObject.setDirty();
    else
      throw new FtsException(new FtsError(FtsError.INSTANTIATION_ERROR, description));

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

  static public FtsSelection getSelection()
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

  /* Utility function: get a Data that is a value of an object property,
     and call and editor on it; but do it asynchroniously;
     the asynchronicity is actually in two places:
     1- to avoid calling Fts.sync, put a property handler on the wished property.

     2- Once the property handler will be called by the input thread, a Runnable
	starting the editor will be posted using invokeLater, so it will be
	executed in the AWT thread.
    */

  static class DelayedEditPropertyHandler implements FtsPropertyHandler
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

    public void propertyChanged(FtsObject object, String name, Object value)
    {
      if (value instanceof MaxData)
	{
	  final MaxData data = (MaxData) value;

	  // Set the document; when documents will be remote data,
	  // it will be handled in FTS.

	  if (data instanceof FtsRemoteData)
	    ((FtsRemoteData) data).setDocument(object.getDocument());

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
	{
	  // If there is nothing to edit, we anyway call the listener
	  // with a null editor, to tell him about the failure

	  listener.editorReady(null); 
	}

      object.removeWatch(this);
    }
  }

  public static void editPropertyValue(FtsObject obj, String property, MaxDataEditorReadyListener listener)
  {
    obj.watch(property, new DelayedEditPropertyHandler(listener));
    obj.ask(property);
  }

  public static void editPropertyValue(FtsObject obj, Object where, String property,
				       MaxDataEditorReadyListener listener)
  {
    obj.watch(property, new DelayedEditPropertyHandler(listener, where));
    obj.ask(property);
  }
}
