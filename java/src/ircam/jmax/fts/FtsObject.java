package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

abstract public class FtsObject implements MaxTclInterpreter
{
  /* code to set generic properties meta-properties */

  static
  {
    // Ins and outs

    FtsPropertyDescriptor.setPersistent("ins", true);
    FtsPropertyDescriptor.setPersistent("outs", true);

    // Name

    FtsPropertyDescriptor.setPersistent("name", true);
    FtsPropertyDescriptor.setDefaultValue("name", "unnamed");

    // Graphic information

    FtsPropertyDescriptor.setPersistent("x", true);
    FtsPropertyDescriptor.setPersistent("y", true);
    FtsPropertyDescriptor.setPersistent("w", true);
    FtsPropertyDescriptor.setPersistent("h", true);

    FtsPropertyDescriptor.setPersistent("wx", true);
    FtsPropertyDescriptor.setPersistent("wy", true);
    FtsPropertyDescriptor.setPersistent("ww", true);
    FtsPropertyDescriptor.setPersistent("wh", true);

    // fonts 

    FtsPropertyDescriptor.setPersistent("font", true);
    FtsPropertyDescriptor.setPersistent("fs", true);
    FtsPropertyDescriptor.setDefaultValue("fs", new Integer(12));
  }

  /******************************************************************************/
  /*                                                                            */
  /*              STATIC FUNCTION                                               */
  /*                                                                            */
  /******************************************************************************/


  /**
   * Static function to build a FtsObject.
   * It is a static method, and
   * not constructor, because the FtsObject created may be of different
   * classes depending on the content of the object.
   *
   * The className can be null; in such case, the description include the class
   * name as first argument; it is usually the case; the description is persistent,
   * and invariant,  it will be saved and retrieved as it is, and correspond to what
   * the user typed.
   *
   * Do not use this function to build patchers, inlets and outlets, message and comment objects;
   * they all have a user accessible constructor in their classes (FtsPatcherObject, FtsInletObject,
   * FtsOutletObject).
   *
   * @param parent the parent object.
   * @param description a string  containing the object description.
   */

  static public FtsObject makeFtsObject(FtsContainerObject parent, String description)
       throws FtsException
  {
    String className;

    className = FtsParse.parseClassName(description);

    return makeFtsObject(parent, className, description);
  }
  
  /** Version with className as explicit argument (possibly doubled in the description) */

  static public FtsObject makeFtsObject(FtsContainerObject parent, String className, String description)
       throws FtsException
  {
    // Add check for declarations, when they exists,
    // comments and message box when description and argument description
    // are merged

    // The check on patcher here is temporary; patchers will move
    // to a specific API as soon as the patcher object is ready.

    if (className.equals("patcher"))
      throw new FtsException(new FtsError(FtsError.INSTANTIATION_ERROR, "patcher"));
    else if (className.equals("inlet"))
      throw new FtsException(new FtsError(FtsError.INSTANTIATION_ERROR, "inlet"));
    else if (className.equals("outlet"))
      throw new FtsException(new FtsError(FtsError.INSTANTIATION_ERROR, "outlet"));
    else if (className.equals("messbox"))
      throw new FtsException(new FtsError(FtsError.INSTANTIATION_ERROR, "messbox"));
    else if (className.equals("comment"))
      throw new FtsException(new FtsError(FtsError.INSTANTIATION_ERROR, "comment"));
    else if (className.equals("slider"))
      return new FtsValueObject(parent, className, description);
    else if (className.equals("intbox"))
      return new FtsValueObject(parent, className, description);
    else if (className.equals("floatbox"))
      return new FtsValueObject(parent, className, description);
    else if (className.equals("toggle"))
      return new FtsValueObject(parent, className, description);
    else if (className.equals("param"))
      return new FtsValueObject(parent, className, description);
    else if (FtsAbstractionTable.exists(className))
      return new FtsAbstractionObject(parent, className, description);
    else if (FtsTemplateTable.exists(className))
      return new FtsTemplateObject(parent, className, description);
    else
      return new FtsStandardObject(parent, className, description);
  }

  /**
   * Static function to redefine a FtsObject.
   * It is static, for
   * similarity with the constructors, because it can produce 
   * a different Java object from the argument, so logically they are not method of the object.
   *
   * It just produce a new object (calling makeFtsObject), and replace the old with it in the 
   * container of the old, and in FTS.
   *
   * You cannot redefine a message, comment, inlet and outlet object, only object that makeFtsObject
   * can actually create.
   *
   * @param obj the object to redefine.
   * @param description  a string containing the description.
   * @return a new object, conforming to the new definition, but with the same FTS
   * identity, and connections; it always create a new object, also if the description
   * is the same
   */

  public static FtsObject redefineFtsObject(FtsObject oldObject, String description) throws FtsException
  {
    FtsObject newObject;
    FtsContainerObject parent;

    parent = oldObject.getParent();
    
    newObject = makeFtsObject(parent, description);

    parent.replace(oldObject, newObject);

    FtsServer.getServer().replaceObject(oldObject, newObject);

    oldObject.delete();

    return newObject;
  }


  /** Static function to get an object by name; it get the 
   *  object by searching it starting from the roots patchers;
   *  the first object with the good name is returned.
   */

  public static FtsObject getObject(String name)
  {
    if (FtsServer.getServer() != null)
      {
	FtsContainerObject root = FtsServer.getServer().getRootObject();
	Vector objects = root.getObjects();

	for (int i = 0; i < objects.size(); i++)
	  {
	    FtsObject ret;
	    FtsObject obj   =  (FtsObject) objects.elementAt(i);

	    if (obj instanceof FtsContainerObject)
	      {
		ret = ((FtsContainerObject) obj).getObjectByName(name);

		if (ret != null)
		  return ret;
	      }
	  }
      }

    return null;
  }

  /******************************************************************************/
  /*                                                                            */
  /*              The property system                                           */
  /*                                                                            */
  /******************************************************************************/


  /* The class PropertyTable handle properties and handlers; it is installed
     only by need, and is an inner class of FtsObject
     */

  class PropertyHandlerTable
  {
    Vector table = new Vector();

    class PropertyHandlerEntry
    {
      String name;
      FtsPropertyHandler handler;
	
      PropertyHandlerEntry(String name, FtsPropertyHandler handler)
      {
	this.name    = name;
	this.handler = handler;
	table.addElement(this);
      }

      void callHandler(String name, Object value)
      {
	if (name.equals(this.name))
	  handler.propertyChanged(FtsObject.this, name, value);
      }
    }

    public void removeWatch(FtsPropertyHandler handler)
    {
      for (int i = 0; i < table.size(); i++)
	{
	  PropertyHandlerEntry ph = (PropertyHandlerEntry) table.elementAt(i);

	  // Shitty code; actually, the handler table should 
	  // not be a vector ... may be a linked list

	  if (ph.handler == handler)
	    {
	      table.removeElement(ph);
	      i--; // to compensate for the shift in the vector
	    }
	}
    }

    public void removeWatch(FtsPropertyHandler handler, String name)
    {
      for (int i = 0; i < table.size(); i++)
	{
	  PropertyHandlerEntry ph = (PropertyHandlerEntry) table.elementAt(i);

	  // Shitty code; actually, the handler table should 
	  // not be a vector ... may be a linked list

	  if ((ph.handler == handler) && (ph.name == name))
	    {
	      table.removeElement(ph);
	      i--; // to compensate for the shift in the vector
	    }
	}
    }

    public void watch(String property, FtsPropertyHandler handler)
    {
      new PropertyHandlerEntry(property, handler);
    }

    synchronized void callHandlers(String name, Object value)
    {
      for (int i = 0; i < table.size(); i++)
	((PropertyHandlerEntry) table.elementAt(i)).callHandler(name, value);
    }
  }

  PropertyHandlerTable propertyHandlerTable = null;

  class Property
  {
    String name;
    Object value;

    Property(String name, Object value)
    {
      this.name = name;
      this.value = value;
    }
  }


  /**
   * Store and access the properties, activating
   * watchers here and in the container if needed.
   * Also "catch" and locally process properties
   * valid for all the objects.
   *
   * Values here have been already parsed from the Tcl
   * command.
   * 
   */

  Vector properties = null;

  public void put(String name, int value)
  {
    put(name, new Integer(value));
  }


  public void put(String name, float value)
  {
    put(name, new Float(value));
  }



  public void put(String name, Object value)
  {
    if (! FtsPropertyDescriptor.isClientOnly(name))
	FtsServer.getServer().putObjectProperty(this, name, value);

    localPut(name, value);
  }

  /** Check if a property correspond to a Java builtin property
   * (Java bean like) and set it; return true in this case.
   * in a far (?) future, this can use BeanInfos instead of handcoded 
   * checks.
   * Subclasses should specialize this method, and finish the else chain by 
   * "return super.builtinPut(name, value)".
   * Handlers are called also for builtin properties.
   *
   * @return true if the property has been set, and should not be stored
   *         in the property list.
   */

  protected boolean builtinPut(String name, Object value)
  {
    if (name.equals("ins"))
      {
	setNumberOfInlets(((Integer)value).intValue());
	return true;
      }
    else if (name.equals("outs"))
      {
	setNumberOfOutlets(((Integer)value).intValue());
	return true;
      }
    else if (name.equals("x"))
      {
	posX = ((Integer)value).intValue();
	return true;
      }
    else if (name.equals("y"))
      {
	posY = ((Integer)value).intValue();
	return true;
      }
    else if (name.equals("w"))
      {
	sizeW = ((Integer)value).intValue();
	return true;
      }
    else if (name.equals("h"))
      {
	sizeH = ((Integer)value).intValue();
	return true;
      }
    else if (name.equals("name"))
      {
	// whatever passed, get it as a string
	setObjectName(value.toString());
	return true;
      }
    else
      return false;
  }

  /** Check if a property correspond to a Java builtin property
   * (Java bean like) and get it.
   * in a far (?) future, this can use BeanInfos instead of handcoded 
   * checks.
   * Subclasses should specialize this method, and finish the else chain by 
   * "return super.builtinGet(name)".
   *
   * @return the value, or null if the property is not builtin
   *
   */

  protected Object builtinGet(String name)
  {
    if (name.equals("ins"))
      return new Integer(getNumberOfInlets());
    else if (name.equals("outs"))
      return new Integer(getNumberOfOutlets());
    else if (name.equals("x"))
      return new Integer(posX);
    else if (name.equals("y"))
      return new Integer(posY);
    else if (name.equals("w"))
      return new Integer(sizeW);
    else if (name.equals("h"))
      return new Integer(sizeH);
    else if (name.equals("name"))
      return getObjectName();
    else
      return null;
  }


  /** Get the names of the "builtin" properties.
   *  Subclasses should add names to the names vector, and 
   *  then call super.builtinPropertyNames(names).
   */

  protected void builtinPropertyNames(Vector names)
  {
    names.addElement("ins");
    names.addElement("outs");
    names.addElement("name");
    names.addElement("x");
    names.addElement("y");
    names.addElement("w");
    names.addElement("h");
  }

  /** Local put is a version of put that do not send
    values to FTS.
    */

  void localPut(String name, Object value)
  {
    // local and hardcoded properties

    if (! builtinPut(name, value))
      {
	if (properties == null)
	  properties = new Vector();

	search :
	  {
	    for (int i = 0; i < properties.size(); i++)
	      {
		Property p = (Property)(properties.elementAt(i));

		if (p.name.equals(name))
		  {
		    // property found, change the value

		    p.value = value;

		    break search;
		  }
	      }

	    // Nothing found, make a new one

	    properties.addElement(new Property(name, value));
	  }
      }

    // Call the handlers 

    if (propertyHandlerTable != null)
      propertyHandlerTable.callHandlers(name, value);


    // Call the handlers in the parent

    if (parent != null)
      parent.callWatchAll(name, value);
  }


  /** Get a propery value */

  public Object get(String name)
  {
    Object v;

    v = builtinGet(name);

    if (v != null)
      return v;
    else
      {
	if (properties != null)
	  {
	    for (int i = 0; i < properties.size(); i++)
	      {
		Property p = (Property)(properties.elementAt(i));
		
		if (p.name.equals(name))
		  return p.value;
	      }
	  }

	return FtsPropertyDescriptor.getDefaultValue(name);
      }
  }

  /** Remove a propery value; not all the properties can
   *  be removed; in particular, remove will not be propagated
   * to FTS, will have no effect on builtin properties, but will
   * anyhow delete the property from the object, and if the property
   * is persistent, it will not be saved.
   */

  public void remove(String prop)
  {
    if (properties != null)
      {
	for (int i = 0; i < properties.size(); i++)
	  {
	    Property p = (Property)(properties.elementAt(i));

	    if (p.name.equals(prop))
	      properties.removeElement(p);
	  }
      }
  }

  public void watch(String property, FtsPropertyHandler handler)
  {
    if (propertyHandlerTable == null)
      propertyHandlerTable = new PropertyHandlerTable();
    
    propertyHandlerTable.watch(property, handler);
  }

  public void removeWatch(FtsPropertyHandler handler)
  {
    if (propertyHandlerTable != null)
      propertyHandlerTable.removeWatch(handler);
  }

  public void removeWatch(FtsPropertyHandler handler, String name)
  {
    if (propertyHandlerTable != null)
      propertyHandlerTable.removeWatch(handler, name);
  }

  public void getPropertyNames(Vector names)
  {
    builtinPropertyNames(names);

    // special properties missing here

    if (properties != null)
      {
	for (int i = 0; i < properties.size(); i++)
	  {
	    Property p = (Property)(properties.elementAt(i));

	    names.addElement(p.name);
	  }
      }
  }


  /******************************************************************************/
  /*                                                                            */
  /*              Object Variables                                              */
  /*                                                                            */
  /******************************************************************************/

  /** Fts Object ID  */

  private int ftsId = -1;

  /** The parent object. Usually the container patcher or abstraction or template */

  FtsContainerObject parent;

  /** The Fts class Name */

  String className;	

  /** The object name, if exists */

  String objectName;

  /** The number of inlets of this object. */

  int ninlets;		    

  /** The number of outlets of this object */

  int noutlets;

  /**
   * True when the object is ready and kept consistent.
   * In an open patcher, don't try to keep it consistent if not ready (sync are expensive !!)
   * ????
   */

  boolean updated = false;

  /**
   * The object string description. 
   * Always set at object creation.
   */

  String description = null;

  // Property and message handlers (to be converted to Beans style)
  // Should go in a structure create by need

  /** The property Handler Table for this object */



  /** The message Handler Table for this object */

  Vector messageTable = null;

  /** on screen representation of the Fts Object */

  Object representation;

  /** Test: x, y, to cache locally the position */

  int posX; // @@@@
  int posY;
  int sizeW;
  int sizeH;

  /** Direct access to geometric properties; recomended when possible */
  /** They don't fire listener, they should !!! */

  public final int getPosX()
  {
    return posX;
  }

  public final void setPosX(int p)
  {
    posX = p;
  }

  public final int getPosY()
  {
    return posY;
  }

  public final void setPosY(int p)
  {
    posY = p;
  }

  public final int getSizeW()
  {
    return sizeW;
  }

  public final void setSizeW(int p)
  {
    sizeW = p;
  }

  public final int getSizeH()
  {
    return sizeH;
  }

  public final void setSizeH(int p)
  {
    sizeH = p;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  FtsObject()
  {
  }

  /**
   * Create a FtsObject object.
   */

  protected FtsObject(FtsContainerObject parent, String className, String description)
  {
    super();

    this.className = className;
    this.description = description;
    this.parent = parent;

    parent.addObjectToContainer(this);
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                               SERVICE LOCAL FUNCTIONS                     */
  /*                                                                           */
  /*****************************************************************************/

  /** Give a string representation of an object. */

  public String toString()
  {
    return "FtsObject:" + ftsId + "{" + description + "}";
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/

  /* Accessors and selectors. */

  /** Get the object including patcher. */

  public final FtsContainerObject getParent()
  {
    return parent;
  }

  /** Get the object class Name. */

  public final String getClassName()
  {
    return className;
  }

  /** Set the objectName */

  void setObjectName(String name)
  {
    this.objectName = name;
  }

  /** Get the objectName */

  public String getObjectName()
  {
    return objectName;
  }

  /** Set the number of inlets */

  public void setNumberOfInlets(int ninlets)
  {
    this.ninlets = ninlets;
  }

  /** Set the number of inlets */

  public void setNumberOfOutlets(int noutlets)
  {
    this.noutlets = noutlets;
  }

  /** Get the complete textual description of the object. */

  public String getDescription()
  {
    return description;
  }

  /** Get the number of inlets of the object */

  public int getNumberOfInlets()
  {
    return ninlets;
  }

  /** Get the number of outlets of the object */

  public int getNumberOfOutlets()
  {
    return noutlets;
  }

  /**
   * Delete the object from fts. 
   * Fts object should be delete explicitely, the finalizer will not delete them.
   */

  public void delete()
  {
    parent.removeObjectFromContainer(this); 
    FtsServer.getServer().freeObject(this);
  }

  // Communication with the object

  /** Send a message to an object (in FTS). */

  public final void sendMessage(int inlet, String selector, Vector args)
  {
    FtsServer.getServer().sendObjectMessage(this, inlet, selector, args);
  }

  /**
   * Install a message handler.
   * All message handlers receive all the messages
   * sent from FTS object to the client.
   */

  public void installMessageHandler(FtsMessageHandler handler)
  {
    if (messageTable == null)
      messageTable = new Vector();

    messageTable.addElement(handler);
  }

  /** Remove a message handler. */

  public void removeMessageHandler(FtsMessageHandler handler)
  {
    if (messageTable != null)
      while (messageTable.removeElement(handler))
	;
  }


  /** Get the representation of this object in the editor. */

  public final Object getRepresentation()
  {
    return representation;
  }

  /** Set the representation of this object in the editor. */

  public final void setRepresentation(Object r)
  {
    representation = r;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               SERVER COMMUNICATION                        */
  /*                                                                           */
  /*****************************************************************************/


  /**
   * Get the fts object id. <p>
   */

  final int getObjId()
  {
    return ftsId;
  }

  /** Set the objid. Private, used only by the server. */

  final void setObjId(int id)
  {
    ftsId = id;
  }


  /** Handle a direct message from an FTS object. 
   * Call the installed handlers.
   */

  void handleMessage(FtsMessage msg)
  {
    if (messageTable != null)
      {
	for (int i = 0; i < messageTable.size(); i++)
	  {
	    FtsMessageHandler hnd = (FtsMessageHandler) messageTable.elementAt(i);

	    hnd.handleMessage(msg);
	  }
      }
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               SAVING                                      */
  /*                                                                           */
  /*****************************************************************************/

  /** An object in a patcher is saved only if this function return true; some 
      objects in the application layer can be not intendend to be save, like
      the Selection itself.
      */

  protected boolean isPersistent()
  {
    return true;
  }

  /** Save the object to a PrintWriter as TCL code; actually defined by the object */

  abstract public void saveAsTcl(PrintWriter writer);


  // May be this two functions should go elsewhere, so that
  // can be used also for connections.

  /** Save the object properties as tcl code; should not print any new line.
   */

  void savePropertiesAsTcl(PrintWriter writer)
  {
    Vector names;
    boolean firstDone = false;

    names = new Vector();

    getPropertyNames(names);

    if (names.size() > 0)
      {
	writer.print(" {");

	for (int i = 0; i < names.size(); i++)
	  {
	    String property;

	    property = (String) names.elementAt(i);

	    // Save all the persistent properties that have a value

	    if (FtsPropertyDescriptor.isPersistent(property))
	      {
		Object value;
		Object defaultValue;
		
		value = this.get(property);
		defaultValue =  FtsPropertyDescriptor.getDefaultValue(property);

		if ((value != null) && ((defaultValue == null) || (! value.equals(defaultValue))))
		  {
		    if (! firstDone)
		      firstDone = true;
		    else
		      writer.print(" ");

		    writer.print(property);
		    writer.print(" ");
		    writer.print(FtsPropertyDescriptor.unparse(property, value));
		  }
	      }
	  }
	
	writer.print("}");
      }
  }

  /**  Take a property list list in the form of a 
   *   TclList, and parse and set all the properties
   */

  public void parseTclProperties(Interp interp, TclObject list) throws FtsException
  {
    try
      {
	int length;

	length = TclList.getLength(interp, list);

	if ((length % 2) == 1)
	  throw new FtsException(new FtsError(FtsError.TPA_ERROR, "in property list"));

	for (int i = 0; i < length; i += 2)
	  {
	    String prop;
	    TclObject obj;

	    prop =  (TclList.index(interp, list, i + 0)).toString();
	    obj  =  TclList.index(interp, list, i + 1);

	    put(prop, FtsPropertyDescriptor.parse(interp, prop, obj));
	  }
      }
    catch (TclException e)
      {
	throw new FtsException(new FtsError(FtsError.TPA_ERROR, "in property list"));
      }
  }

  /** The Tcl eval for an object; just eval the script, for the basic case
   */


  public void eval(Interp interp, String  script) throws tcl.lang.TclException
  {
    eval(interp, TclString.newInstance(script));
  }

  /** as the previous one, getting the script as tclObject */

  public void eval(Interp interp, TclObject script) throws tcl.lang.TclException
  {
    interp.eval(script, 0);
  }
}









