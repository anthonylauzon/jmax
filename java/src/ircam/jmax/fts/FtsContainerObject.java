package ircam.jmax.fts;

import java.io.*;
import java.util.*;
import tcl.lang.*;

import ircam.jmax.*;

/** 
 *  This is the super class of all
 * objects that include other objects, like
 * patchers, template and abstractions.
 * 
 * This is a specialization of the FtsAbstractContainer that 
 * necessarly represent a patcher in the FTS meaning; i.e. 
 * a real patcher or template/abstraction instance.
 * So it add inlets/outlets housekeeping, patcher opening, patcher after load init,
 * patcher relative properties, script evaluation, .pat file support, find and object
 * naming support.
 */

abstract public class FtsContainerObject extends FtsAbstractContainerObject
{
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

  public void setNumberOfInlets(int ninlets)
  {
    // delete unneeded inlets 

    for (int i = ninlets; i < this.ninlets; i++)
      {
	FtsObject obj;

	obj = (FtsObject) inlets.elementAt(i);
	obj.delete();
      }

    super.setNumberOfInlets(ninlets);
    this.inlets.setSize(ninlets);
  }

  /** Set the number of inlets */

  public void setNumberOfOutlets(int noutlets)
  {
    // delete unneeded inlets 

    for (int i = noutlets; i < this.noutlets; i++)
      {
	FtsObject obj;

	obj = (FtsObject) outlets.elementAt(i);
	obj.delete();
      }

    super.setNumberOfOutlets(noutlets);
    this.outlets.setSize(noutlets);
  }


  /** Update the FTS object; redefined it to have the
   * correct name, ninlets and noutlets; the fts object for a container
   * is always a patcher.
   */

  public void updateFtsObject()
  {
    FtsServer.getServer().redefinePatcherObject(this, objectName, ninlets, noutlets);
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
  
  /** Remove an inlet from the container */

  final void removeInlet(FtsInletObject in, int pos)
  {
    if (pos < ninlets)
      inlets.setElementAt(null, pos);
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

  /** Remove an outlet from the container */

  final void removeOutlet(FtsOutletObject out, int pos)
  {
    if (pos < ninlets)
      inlets.setElementAt(null, pos);
  }

  /** Open tell FTS that this patcher is "alive" */

  public final void open()
  {
    open = true;
    FtsServer.getServer().openPatcher(this);
    FtsServer.getServer().syncToFts();
  }

  /** Close tell FTS that this patcher is not "alive". */

  public final void close()
  {
    open = false;

    FtsServer.getServer().closePatcher(this);
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
    FtsServer.getServer().patcherLoaded(this);
  }


  /** Keep the container handlers */

  FtsObject.PropertyHandlerTable containerPropertyHandlerTable = null;

  /** Watch a property of all the object in the patcher 
   *
   */

  public void watchAll(String property, FtsPropertyHandler handler)
  {
    if (containerPropertyHandlerTable == null)
      containerPropertyHandlerTable = new PropertyHandlerTable();
    
    containerPropertyHandlerTable.watch(property, handler);
  }

  /** Remove a watch all */

  public void removeWatchAll(FtsPropertyHandler handler)
  {
    if (containerPropertyHandlerTable != null)
      containerPropertyHandlerTable.removeWatch(handler);
  }

  /** execute the watch all */

  void callWatchAll(String property, Object value)
  {
    if (containerPropertyHandlerTable != null)
      containerPropertyHandlerTable.callHandlers(property, value);
  }

  /** The Tcl eval for an object; just eval the script, but handle
   *  a globally available container stack.
   */

  public static Stack containerStack = new Stack(); // should not be public !


  public void eval(Interp interp, TclObject script) throws tcl.lang.TclException
  {
    containerStack.push(this);

    // We need to call _BasicThisWrapper because in this way we
    // provide a "local" environment to the code being executed, i.e. al
    // the variables will be locals

    TclObject list = TclList.newInstance();

    TclList.append(interp, list, TclString.newInstance("_BasicThisWrapper"));
    TclList.append(interp, list, ReflectObject.newInstance(interp, this));
    TclList.append(interp, list, script);

    interp.eval(list, 0);

    containerStack.pop();
  }


  /** Selection
   * Get the unique FtsSelect object for this container
   */

  FtsSelection selection = null;

  public FtsSelection getSelection()
  {
    if (selection == null)
      selection = new FtsSelection(this);
    
    return selection;
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
	  int ix = ((Integer) ((FtsObject) inlets.elementAt(i)).get("x")).intValue();
	  int jx = ((Integer) ((FtsObject) inlets.elementAt(j)).get("x")).intValue();

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
	  int ix = ((Integer) ((FtsObject) outlets.elementAt(i)).get("x")).intValue();
	  int jx = ((Integer) ((FtsObject) outlets.elementAt(j)).get("x")).intValue();

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


  /*****************************************************************************/
  /*                                                                           */
  /*                    Object Naming                                          */
  /*                                                                           */
  /*****************************************************************************/

  /** Get an object by name; a name is either a single name or a composed
   *  name; a composed name is interpreted relatively to this container.
   */


  public FtsObject getObjectByName(String name)
  {
    if (name.indexOf('.') == -1)
      return getObjectBySimpleName(name);
    else
      {
	String rootName;
	String tailName;
	FtsObject obj;
	
	rootName = name.substring(0, name.indexOf('.'));
	tailName = name.substring(name.indexOf('.') + 1);

	obj = getObjectBySimpleName(rootName);

	if (obj instanceof FtsContainerObject)
	  return ((FtsContainerObject) obj).getObjectByName(tailName);
	else
	  return null;
      }
  }

  final private FtsObject getObjectBySimpleName(String name)
  {
    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	String objName  =  (String) obj.get("name");

	if ((objName != null) && (objName.equals(name)))
	  return obj;
      }

    return null;
  }
}







