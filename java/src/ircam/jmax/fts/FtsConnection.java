package ircam.jmax.fts;

import ircam.jmax.*;
import java.io.*;
import java.util.*;

/**
 * Class implementing the proxy of an Fts connection in the client
 * deals with the connection creation/deletion and saving/loading
 */

public class FtsConnection 
{
  /** to protect against double deletion.
    temporary, waiting for a more event/based editor
    */

  private int id;

  boolean deleted = false; 
  FtsObject from;
  int outlet;

  FtsObject to;
  int inlet;

  /** Create a FTS connection.  */

  FtsConnection(int id, FtsObject from, int outlet, FtsObject to, int inlet)
  {
    super();
   
    this.id     = id;
    this.from   = from;
    this.outlet = outlet;
    this.to     = to;
    this.inlet  = inlet;

    from.getParent().addConnectionToContainer(this);
    from.setDirty();
  }

  /**
   * Get the fts connection id. <p>
   */

  final int getConnectionId()
  {
    return id;
  }

  /** Set the objid. Private, used only by the server. */

  final void setConnectionId(int id)
  {
    this.id = id;
  }


  /** Replace one object with the other in the connection
   *  valid only if we are doing a replace on the FTS side, also,
   *  this do not change the connections in FTS; this is why this method
   * is not public
   */

  void replace(FtsObject oldObject, FtsObject newObject)
  {
    if ((from == oldObject) || (to == oldObject))
      {
	Fts.getServer().deleteConnection(this);

	if (from == oldObject)
	  from = newObject;

	if (to == oldObject)
	  to = newObject;

	Fts.getServer().newConnection(id, from, outlet, to, inlet);
      }
  }

  /** Undo the connection. */

  public void delete()
  {
    if (deleted)
      return;

    deleted = true;
    from.setDirty();
    to.setDirty();
	
    Fts.getServer().deleteConnection(this);

    from.getParent().removeConnectionFromContainer(this);
  }

  /** Access the From. The From is the FtsObject origin of the connection. */

  public FtsObject getFrom()
  {
    return from;
  }

  /** Access the To. The To is the FtsObject destination of the connection. */

  public FtsObject getTo()
  {
    return to;
  }

  /** Access the From outlet. */

  public int getFromOutlet()
  {
    return outlet;
  }

  /** Access the To inlet. */

  public int getToInlet()
  {
    return inlet;
  }

  /** Save support. Save the connection as a TCL command. */

  void saveAsTcl(PrintWriter writer)
  {
    writer.print("connection $obj(" + from.getObjectId() + ") " + outlet + " $obj(" + to.getObjectId() + ") " + inlet);
  }

  public String  toString()
  {
    return "FtsConnection(" + from + "," + outlet + "," + to + "," + inlet + ")";
  }
}






