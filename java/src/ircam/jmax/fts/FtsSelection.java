package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;


import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * Specialization of the abstract container to implement
 * the selection.
 * 
 */

public class FtsSelection extends FtsAbstractContainerObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */

  FtsSelection(FtsContainerObject parent)
  {
    super(parent, "selection", "");

    ninlets = 0;
    noutlets = 0;
  }

  /** Add an object to this container  */

  final public void addObject(FtsObject obj)
  {
    addObjectToContainer(obj);
  }

  /** Remove an object from this container. */

  final public void removeObject(FtsObject obj)
  {
    removeObjectFromContainer(obj);
  }

  /** Add a connection to this selection. */

  final public void addConnection(FtsConnection obj)
  {
    addConnectionToContainer(obj);
  }

  /** Remove an connection from this container. */

  final void removeConnection(FtsConnection obj)
  {
    removeConnectionFromContainer(obj);
  }

  /** clean: remove everything */

  final public void clean()
  {
    objects.removeAllElements();
    connections.removeAllElements();
  }

  /** This object is not visible */

  public boolean isRepresented()
  {
    return false;
  }

  /** This object is not persistent */

  protected boolean isPersistent()
  {
    return false;
  }

  /** Since this object is not persistent, this function is
    not called to save it as a part of a .tpa file, 
    but it is called for copy/paste operations
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






