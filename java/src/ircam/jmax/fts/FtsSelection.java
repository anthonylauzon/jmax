package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;


import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * Implement the selection; for now, without listener, and
 * local to each patcher; later, unique, and more integrated with FTS.
 * 
 */

public class FtsSelection 
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /** The objects contained in the patcher */

  private Vector objects     = new Vector();

  /** All the connections between these objects */

  private Vector connections = new Vector();


  /**
   * Create a FtsObject object;
   */

  FtsSelection()
  {
  }

  /** Add an object to this container  */

  final public void addObject(FtsObject obj)
  {
    objects.addElement(obj);
  }

  /** Remove an object from this container. */

  final public void removeObject(FtsObject obj)
  {
    objects.removeElement(obj);
  }

  /** Get the objects */

  final public Vector getObjects()
  {
    return objects;
  }

  /** Add a connection to this selection. */

  final public void addConnection(FtsConnection obj)
  {
    connections.addElement(obj);
  }

  /** Remove an connection from this container. */

  final void removeConnection(FtsConnection obj)
  {
    connections.removeElement(obj);
  }

  /** Get the connections */

  final public Vector getConnections()
  {
    return connections;
  }

  /** clean: remove everything */

  final public void clean()
  {
    objects.removeAllElements();
    connections.removeAllElements();
  }


  /** this method is
   * called for copy/paste operations
    */

  public void saveAsTcl(PrintWriter writer)
  {
    // Write the body 
    //
    // First, store the declarations; declaration don't have
    // connections, so we don't store them in variables.

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	
	if (obj instanceof FtsDeclarationObject)
	  {
	    obj.saveAsTcl(writer);
	    writer.println();
	  }
      }

    // Then store the objects

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	
	if (! (obj instanceof FtsDeclarationObject))
	  {
	    writer.print("set obj(" + obj.getObjId() + ")" + " [");

	    obj.saveAsTcl(writer);

	    writer.println("]");
	  }
      }

    // Then, store the connections
    // Connections are only stored if both ends are in the selection !

    for (int i = 0; i < connections.size(); i++)
      {
	FtsConnection c   =  (FtsConnection) connections.elementAt(i);

	if (objects.contains(c.to) && objects.contains(c.from))
	  {
	    c.saveAsTcl(writer);
	    writer.println();
	  }
      }
  }
}






