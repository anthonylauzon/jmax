
package ircam.jmax.fts;

import ircam.jmax.*;
import java.io.*;
import java.util.*;

/**
 * Implement the content of a patcher/container. <p>
 *
 * A FtsPatcher contain a list of Objects and a list of Connections,
 * and refer to an instance of FtsObject representing the container object 
 * itself.
 *
 * Some of its functionalities should/will move to a subclass of FtsObject.
 *
 * @see FtsObject
 */

class FtsPatcher
{
  /** The window description for the container */

  FtsWindowDescription windowDescr;

  /** The name of the container */

  String name;

  /** The objects contained in the patcher */

  Vector objects     = new Vector();

  /** All the connections between these objects */

  Vector connections = new Vector();

  /** The objects that are also inlets */

  Vector inlets      = new Vector();

  /** The objects that are also outlets */

  Vector outlets     = new Vector();

  /** The number of inlets this container have */

  int ninlets  = 0;

  /** The number of outlets this container have */

  int noutlets = 0;

  /** True if the patcher is Open in FTS */
  boolean open = false;

  /** the actual FtsObject implementing the container */

  FtsObject  obj;	


  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /** Build a new container object */

  FtsPatcher(FtsObject obj, String name, int ninlets, int noutlets)
  {
    super();

    this.name    = name;
    this.ninlets = ninlets;
    this.noutlets = noutlets;
    this.inlets.setSize(ninlets);
    this.outlets.setSize(noutlets);

    this.obj = obj;
  }


  /** Set the FtsObject. */

  void setObject(FtsObject obj)
  {
    this.obj = obj;
  }

  /** Get the FtsObject. */

  FtsObject getObject()
  {
    return obj;
  }

  /** Redefine the number of inlets and outlets of the container. */

  void redefine(int ninlets, int noutlets)
  {
    this.ninlets = ninlets;
    this.noutlets = noutlets;
    this.inlets.setSize(ninlets);
    this.outlets.setSize(noutlets);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               SERVICE LOCAL FUNCTIONS                     */
  /*                                                                           */
  /*****************************************************************************/

  /** Give a String representation of the container.*/

  public String toString()
  {
    return "FtsPatcher<" + obj.getArguments() + ">";
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               SERVICE FAL FUNCTIONS                       */
  /*                                                                           */
  /*****************************************************************************/

  // Structure housekeeping

  /** Add an object to the patcher object list. */

  void addObject(FtsObject obj)
  {
    objects.addElement(obj);
  }

  /** Remove an object from the patcher object list. */

  void removeObject(FtsObject obj)
  {

    objects.removeElement(obj);
  }

  /** Add an connection to the patcher connection list. */

  void addConnection(FtsConnection obj)
  {
    connections.addElement(obj);
  }

  /** Remove an connection from the patcher connection list. */

  void removeConnection(FtsConnection obj)
  {
    connections.removeElement(obj);
  }

  /** Add an inlet. */

  void addInlet(FtsObject in, int pos)
  {
    if (pos > (ninlets - 1))
      {
	Vector args = new Vector();

	args.addElement(name);
	args.addElement(new Integer(pos+1));
	args.addElement(new Integer(noutlets));

	obj.setArguments(args);
      }

    inlets.setElementAt(in, pos);
  }

  /**
   * Add an inlet.
   * This version of addInlet just add the inlet at the
   * end; it is used for .pat compatibility; the order is
   * computed later, by sorting the inlets
   */

  void addInlet(FtsObject in)
  {
    inlets.addElement(in);
  }

  /** Add an oulet. */

  void addOutlet(FtsObject out, int pos)
  {
    if (pos > (noutlets - 1))
      {
	Vector args = new Vector();

	args.addElement(name);
	args.addElement(new Integer(ninlets));
	args.addElement(new Integer(pos+1));

	obj.setArguments(args);
      }

    outlets.setElementAt(out, pos);
  }
 
  /**
   * Add an outlet.
   * This version of addOutlet just add the outlet at
   * the end; it is used for .pat compatibility; the order is computed
   * later, by sorting the outlets
   */

  void addOutlet(FtsObject in)
  {
    outlets.addElement(in);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                    Find support                                           */
  /*                                                                           */
  /*****************************************************************************/

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

	if (obj.isContainer())
	  {
	    obj.getSubPatcher().find(pattern, v);
	  }
      }

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
  
  void assignInOutletsAndName(String name)
  {
    Vector args = new Vector();

    // First, redefine the patcher with the right number of 
    // inlets and outlets

    args.addElement(name);
    args.addElement(new Integer(inlets.size()));
    args.addElement(new Integer(outlets.size()));
    obj.setArguments(args);

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
      {
	FtsObject inlet;

	inlet = (FtsObject) inlets.elementAt(i);
	inlet.args.addElement(new Integer(i));

	inlet.setArguments(inlet.args);
      }
	

    for (int i = 0; i < noutlets; i++)
      {
	FtsObject outlet;

	outlet = (FtsObject) outlets.elementAt(i);
	outlet.args.addElement(new Integer(i));
	
	outlet.setArguments(outlet.args);
      }
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CLIENT API                                  */
  /*                                                                           */
  /*****************************************************************************/


  /** Get the connections. */

  Vector getConnections()
  {
    return connections;
  }

  /** Get the objects. */

  Vector getObjects()
  {
    return objects;
  }

  /** Open tell FTS that this patcher is "alive" */

  void open()
  {
    open = true;
    MaxApplication.getFtsServer().openPatcher(obj);
    MaxApplication.getFtsServer().syncToFts();
  }

  /** Close tell FTS that this patcher is not "alive". */

  void close()
  {
    open = false;

    MaxApplication.getFtsServer().closePatcher(obj);
  }

  /** Check if the patcher is open. */

  boolean isOpen()
  {
    return open;
  }

  /**
   * Loaded tell FTS that this patcher has been loaded, and
   *  post-load initialization should be fired.
   */

  void loaded()
  {
    MaxApplication.getFtsServer().patcherLoaded(obj);
  }

  /** Get the window description. */

  FtsWindowDescription getWindowDescription()
  {
    return windowDescr;
  }

  /** Set the window description. */

  void setWindowDescription(FtsWindowDescription w)
  {
    windowDescr = w;
  }

  /** Save to an output stream */

  void saveTo(OutputStream stream)
  {
    FtsSaveStream fs;

    MaxApplication.getFtsServer().syncToFts();
    fs = new FtsSaveStream(stream);

    // Add the root patcher to the document

    fs.print("# Top Level Patcher");
    fs.println();
    fs.println();
    fs.print("set objs(" + obj.idx + ") [");
    obj.saveAsTcl(fs);
    fs.print("]");
    fs.println();

    saveAsTcl(fs, 1);

    // Add document building command

    fs.print("show $objs("+ obj.idx + ")");
    fs.println();

    // Add the load init command

    fs.print("init $objs("+ obj.idx + ")");
    fs.println();

    // flush

    fs.flush();
  }


  /**
   * Saving to a printStream.
   * It save the content of the patcher,
   * declaration first, object and then connection as TCL commands. <p>
   *
   * The commands store the result of the computation in a objs
   * array, using a global index of the object that is incremented
   * and passed around recursively.
   * The index is stored in the objects to speed up 
   * this array is used to generate the connect commands. <p>
   *
   * Then, the subpatchers are saved; the patcher object is referred
   * by its index. <p>
   *
   * The FtsSaveStream simply handle indentation of the commands
   * to improve, a little, the readability of the file.
   */

  int saveAsTcl(FtsSaveStream stream, int idxCount)
  {
    // First, compute store the indexes in the objects,
    // so we don't need quadratic searchs for the connections

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);

	obj.idx = idxCount++;
      }
    stream.indentMore();

    stream.println();
    stream.print("# Patcher Content");
    stream.println();
    stream.println();


    // Then, store the declarations; declaration don't have
    // connections, so we don't store them in variables.

    boolean no_declarations = true;

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	
	if (obj.declaration)
	  {
	    if (no_declarations)
	      {
		stream.print("# Declarations");
		stream.println();
		stream.println();
		no_declarations = false;
	      }
		
	    obj.saveAsTcl(stream);
	    stream.println();
	  }
      }

    if (! no_declarations)
      stream.println();

    // Then store the objects

    boolean no_objects = true;

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	
	if (! obj.declaration)
	  {
	    if (no_objects)
	      {
		stream.print("# Objects");
		stream.println();
		stream.println();
		no_objects = false;
	      }

	    stream.print("set objs(" + obj.idx + ")" + " [");

	    obj.saveAsTcl(stream);

	    stream.print("]");
	    stream.println();
	  }
      }

    if (! no_objects)
      stream.println();

    // Then, store the connections

    if (connections.size() > 0)
      {
	stream.print("# Connections");
	stream.println();
	stream.println();
      }

    for (int i = 0; i < connections.size(); i++)
      {
	FtsConnection c   =  (FtsConnection) connections.elementAt(i);
	
	c.saveAsTcl(stream);
	stream.println();
      }

    stream.println();

    // Then, store all the subpatcher that are not abstraction 
    // (this should done better !!!!)

    boolean no_subpatchers = true;

    for (int i = 0; i < objects.size(); i++)
      {
	FtsObject obj   =  (FtsObject) objects.elementAt(i);
	
	if ((! obj.isTemplate()) && (! obj.isAbstraction()) && (obj.getSubPatcher() != null)) 
	  {
	    if (no_subpatchers)
	      {
		stream.print("# Subpatchers");
		stream.println();
		stream.println();
		no_subpatchers = false;
	      }

	    /* idxCount = obj.getSubPatcher().saveAsTcl(stream, idxCount); */

	    // May be we don't really need to update the idxCount; we can reuse
	    // old idx, so to reuse Tcl variables, and reduce Tcl memory space.
	    
	    obj.getSubPatcher().saveAsTcl(stream, idxCount);
	  }
      }

    // End of patcher

    stream.indentLess();

    if (! no_subpatchers)
      stream.println();

    stream.print("# End Patcher");
    stream.println();
    stream.println();

    return idxCount;
  }
}













