package ircam.jmax.fts;

import java.util.*;

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

    server.syncToFts();

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

    server.syncToFts();
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

    server.syncToFts();

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

    server.syncToFts();
    newObject = server.getObjectByFtsId(id);
    
    if (newObject != null)
      newObject.setDirty();
    else
      throw new FtsException(new FtsError(FtsError.INSTANTIATION_ERROR, description));

    return newObject;
  }


  /** Static function to get an object by name; it get the 
   *  object by searching it starting from the roots patchers;
   *  the first object with the good name is returned.
   */

  public static FtsObject getObject(String name)
  {
    if (Fts.getServer() != null)
      {
	FtsContainerObject root = Fts.getServer().getRootObject();
	Vector objects = root.getObjects();

	for (int i = 0; i < objects.size(); i++)
	  {
	    FtsObject ret;
	    FtsObject obj   =  (FtsObject) objects.elementAt(i);

	    if (obj instanceof FtsContainerObject)
	      {
		ret = ((FtsContainerObject) obj).getObjectByName(name);

		if (ret != null)
		  return ret;
	      }
	  }
      }

    return null;
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
}
