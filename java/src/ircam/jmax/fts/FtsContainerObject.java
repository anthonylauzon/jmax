package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/** 
 *  This is the super class of all
 * objects that include other objects, like
 * patchers, template and abstractions.
 */

abstract public class FtsContainerObject extends FtsObject
{
  /** The window description for the container */

  FtsWindowDescription windowDescription = null;

  /** The objects contained in the patcher */

  Vector objects     = new Vector();

  /** All the connections between these objects */

  Vector connections = new Vector();

  /** The objects that are also inlets */

  Vector inlets      = new Vector();

  /** The objects that are also outlets */

  Vector outlets     = new Vector();

  /** True if the patcher is Open in FTS */

  boolean open = false;

  /** Just for the building of the root object */

  FtsContainerObject()
  {
    super();
  }

  protected  FtsContainerObject(FtsContainerObject parent, String className, String description)
  {
    super(parent, className, description);
  }

  /** Set the number of inlets */

  final void setNumberOfInlets(int ninlets)
  {
    this.ninlets = ninlets;
    this.inlets.setSize(ninlets);
  }

  /** Set the number of inlets */

  final void setNumberOfOutlets(int noutlets)
  {
    this.noutlets = noutlets;
    this.outlets.setSize(noutlets);
  }


  /** Update the FTS object; redefined it to have the
   * correct name, ninlets and noutlets; the fts object for a container
   * is always a patcher.
   */

  final void updateFtsObject()
  {
    MaxApplication.getFtsServer().redefinePatcherObject(this, name, ninlets, noutlets);
  }

  /** Add an object to this container  */

  final void addObject(FtsObject obj)
  {
    objects.addElement(obj);
  }

  /** Remove an object from this container. */

  final void removeObject(FtsObject obj)
  {
    objects.removeElement(obj);
  }

  /** Add an connection to this container. */

  final void addConnection(FtsConnection obj)
  {
    connections.addElement(obj);
  }

  /** Remove an connection from this container. */

  final void removeConnection(FtsConnection obj)
  {
    connections.removeElement(obj);
  }

  /** Add an inlet. */

  final void addInlet(FtsInletObject in, int pos)
  {
    if (pos > (ninlets - 1))
      {
	setNumberOfInlets(pos+1);
	updateFtsObject(); // update the corresponding FTS object
      }

    inlets.setElementAt(in, pos);
  }

  /**
   * Add an inlet.
   * This version of addInlet just add the inlet at the
   * end; it is used for .pat compatibility; the order is
   * computed later, by sorting the inlets
   */

  final void addInlet(FtsInletObject in)
  {
    inlets.addElement(in);
  }

  /** Add an oulet. */

  final void addOutlet(FtsObject out, int pos)
  {
    if (pos > (noutlets - 1))
      {
	setNumberOfOutlets(pos+1);
	updateFtsObject(); // update the corresponding FTS object
      }

    outlets.setElementAt(out, pos);
  }
 
  /**
   * Add an outlet.
   * This version of addOutlet just add the outlet at
   * the end; it is used for .pat compatibility; the order is computed
   * later, by sorting the outlets
   */

  final void addOutlet(FtsObject in)
  {
    outlets.addElement(in);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CLIENT API                                  */
  /*                                                                           */
  /*****************************************************************************/


  /** Get the connections. */

  public final Vector getConnections()
  {
    return connections;
  }

  /** Get the objects. */

  public final Vector getObjects()
  {
    return objects;
  }

  /** Open tell FTS that this patcher is "alive" */

  public final void open()
  {
    open = true;
    MaxApplication.getFtsServer().openPatcher(this);
    MaxApplication.getFtsServer().syncToFts();
  }

  /** Close tell FTS that this patcher is not "alive". */

  public final void close()
  {
    open = false;

    MaxApplication.getFtsServer().closePatcher(this);
  }

  /** Check if the patcher is open. */

  public final boolean isOpen()
  {
    return open;
  }

  /**
   * Loaded tell FTS that this patcher has been loaded, and
   *  post-load initialization should be fired.
   */

  public final void loaded()
  {
    MaxApplication.getFtsServer().patcherLoaded(this);
  }

  /** Get the window description. */

  public final FtsWindowDescription getWindowDescription()
  {
    return windowDescription;
  }

  /** Set the window description. */

  public final void setWindowDescription(FtsWindowDescription w)
  {
    windowDescription = w;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                    Special support for .pat files                         */
  /*                                                                           */
  /*****************************************************************************/

  /**
    Redefine the patcher and then assign the inlets and outlets in the good positions 
    should be used *only* for "fixing" patcher loaded by the FtsDotPatParser.
    */
  
  void assignInOutlets()
  {
    // First, compute the actual size of inlets and outlets arrays
    // and assign as number of inlets and outlets

    ninlets  = inlets.size();
    noutlets = outlets.size();

    // First, update the FTS object 

    updateFtsObject();

    // Then, sort the inlets and the outlets based on their x
    // position

    for (int i = 1; i < ninlets; i++)
      for (int j = 0; j < i; j++)
	{
	  int ix = ((FtsObject) inlets.elementAt(i)).getGraphicDescription().x;
	  int jx = ((FtsObject) inlets.elementAt(j)).getGraphicDescription().x;

	  if (jx > ix)
	    {
	      Object tmp;

	      tmp = inlets.elementAt(i);
	      inlets.setElementAt(inlets.elementAt(j), i);
	      inlets.setElementAt(tmp, j);
	    }
	}

    for (int i = 1; i < noutlets; i++)
      for (int j = 0; j < i; j++)
	{
	  int ix = ((FtsObject) outlets.elementAt(i)).getGraphicDescription().x;
	  int jx = ((FtsObject) outlets.elementAt(j)).getGraphicDescription().x;

	  if (jx > ix)
	    {
	      Object tmp;

	      tmp = outlets.elementAt(i);
	      outlets.setElementAt(outlets.elementAt(j), i);
	      outlets.setElementAt(tmp, j);
	    }
	}

    // Then, redefine the existing inlets and outlets
    // with the computed position

    for (int i = 0; i < ninlets; i++)
      ((FtsInletObject) inlets.elementAt(i)).setPosition(i);
	
    for (int i = 0; i < noutlets; i++)
      ((FtsOutletObject) outlets.elementAt(i)).setPosition(i);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                    Special support for find                               */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Find all the objects that have a given pattern 
   * in its descriptions.
   */

  public Vector find(String pattern)
  {
    Vector v = new Vector();

    find(pattern, v);

    return v;
  }

  /**
   * Find all the objects that have a given pattern 
   * in its descriptions.
   */

  void find(String pattern, Vector v)
  {
    // We do two loops to find all the objects in a patcher
    // before looking in the contained patcher.

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	
	if (obj.getDescription().indexOf(pattern) != -1)
	  v.addElement(obj);
      }

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);

	if (obj instanceof FtsContainerObject)
	  {
	    ((FtsContainerObject) obj).find(pattern, v);
	  }
      }
  }
}







