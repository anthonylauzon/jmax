package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * Implement the selection; for now, without listener, and
 * local to each patcher; later, unique, and more integrated with FTS.
 * 
 */

public class FtsSelection  extends FtsObject
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
   * Create a Fts selection;
   */

  protected  FtsSelection(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);
  }

  /** Add an object to this container  */

  final public void addObject(FtsObject obj)
  {
    objects.addElement(obj);

    Fts.getServer().sendObjectMessage(this, -1, "add_object", obj);
  }

  /** Remove an object from this container. */

  final public void removeObject(FtsObject obj)
  {
    if (objects.contains(obj))
      {
	objects.removeElement(obj);
	Fts.getServer().sendObjectMessage(this, -1, "remove_object", obj);
      }
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

    Fts.getServer().sendObjectMessage(this, -1, "add_connection", obj);
  }

  /** Remove an connection from this container. */

  final public void removeConnection(FtsConnection obj)
  {
    if (connections.contains(obj))
      {
	connections.removeElement(obj);
	Fts.getServer().sendObjectMessage(this, -1, "remove_connection", obj);
      }
  }

  /** Get the connections */

  final public Vector getConnections()
  {
    return connections;
  }

  /** clean: remove everything */

  final public void clean()
  {
    sendMessage(-1, "clear", null);

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
	    writer.print("set obj(" + obj.getObjectId() + ")" + " [");

	    obj.saveAsTcl(writer);

	    writer.println("]");

 	    if ((! (obj instanceof FtsContainerObject)) && (obj instanceof FtsObjectWithData))
 	      {
 		FtsDataObject data;

		data = (FtsDataObject) ((FtsObjectWithData) obj).getData();
		    
		writer.print("setData $obj(" + obj.getObjectId() + ")" + " [");
		data.saveAsTcl(writer);
		writer.println("]");
	      }
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






