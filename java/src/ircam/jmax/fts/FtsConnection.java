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
  /** on screen representation of the Fts Connection */

  Object representation = null;

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
  }

  void redefine(FtsObject from, int outlet, FtsObject to, int inlet)
  {
    this.from   = from;
    this.outlet = outlet;
    this.to     = to;
    this.inlet  = inlet;
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

  /** Undo the connection. */

  public void delete()
  {
    if (deleted)
      return;

    deleted = true;

    Fts.getSelection().removeConnection(this);

    from.setDirty();
    to.setDirty();
	
    Fts.getServer().deleteConnection(this);

    from.getParent().removeConnectionFromContainer(this);
  }


  /** Undo the connection, only the client part */

  public void release()
  {
    if (deleted)
      return;

    deleted = true;

    Fts.getSelection().removeConnection(this);

    from.setDirty();
    to.setDirty();
	
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

  public String  toString()
  {
    return "FtsConnection(" + from + "," + outlet + "," + to + "," + inlet + ", #" + id + ")";
  }

  /** Get the representation of this connection in the editor. */

  public final Object getRepresentation()
  {
    return representation;
  }

  /** Set the representation of this connection in the editor. */

  public final void setRepresentation(Object r)
  {
    representation = r;
  }
}






