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
  FtsObject from;
  int outlet;

  FtsObject to;
  int inlet;

  /** Create a FTS connection.  */

  public FtsConnection(FtsObject from, int outlet, FtsObject to, int inlet)
  {
    super();
   
    this.from   = from;
    this.outlet = outlet;
    this.to     = to;
    this.inlet  = inlet;

    FtsServer.getServer().connectObjects(from, outlet, to, inlet);

    from.getParent().addConnectionToContainer(this);
  }


  /** Replace one object with the other in the connection
   *  valid only if we are doing a replace on the FTS side, also,
   *  this do not change the connections in FTS; this is why this method
   * is not public
   */

  void replace(FtsObject oldObject, FtsObject newObject)
  {
    if (from == oldObject)
      from = newObject;

    if (to == oldObject)
      to = newObject;
  }

  /** Undo the connection. */

  public void delete()
  {
    FtsServer.getServer().disconnectObjects(from, outlet, to, inlet);

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
    writer.print("connection $obj(" + from.getObjId() + ") " + outlet + " $obj(" + to.getObjId() + ") " + inlet);
  }

  /**
   * Check the  consistency of the connection.
   * Return true if it is consistent with the
   * status of the objects, false otherwise; i.e. it check that
   * the inlets actually exists; it may happen that we get an error
   * in an object instantiation, and the connection cannot be done ...
   * It work only if the patcher is open, otherwise always return true.
   */

  public boolean checkConsistency()
  {
    if (from.getParent().isOpen())
      {
	if (inlet >= to.getNumberOfInlets())
	  {
	    System.out.println("Cannot connect to inlet " + inlet + " of object " + to);
	    return false;
	  }

	if (outlet >= from.getNumberOfOutlets())
	  {
	    System.out.println("Cannot connect to outlet " + outlet + " of object " + from);
	    return false;
	  }
      }

    return true;
  }

  public String  toString()
  {
    return "FtsConnection(" + from + "," + outlet + "," + to + "," + inlet + ")";
  }
}






