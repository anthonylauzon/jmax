package ircam.jmax.fts;

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

  /** Selection handling */

  /** Selection
   * Get the unique FtsSelect object for this container
   */

  private static FtsSelection selection = null;

  static public FtsSelection getSelection()
  {
    if (selection == null)
      selection = new FtsSelection();
    
    return selection;
  }
}
