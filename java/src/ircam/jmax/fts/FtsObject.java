package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;
import java.text.*;

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
  static private NumberFormat numberFormat;

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
    FtsPropertyDescriptor.setDefaultValue("fs", new Integer(10));

    // Number format for messages coming from FTS (to be cleaned up:
    // the text should be sent by FTS as text alread).
    
    numberFormat = NumberFormat.getInstance(Locale.US);
    numberFormat.setMaximumFractionDigits(6);
    numberFormat.setMinimumFractionDigits(1);
    numberFormat.setGroupingUsed(false);
  }

  /******************************************************************************/
  /*                                                                            */
  /*              STATIC FUNCTION                                               */
  /*                                                                            */
  /******************************************************************************/

  /** This version create an application layer object for an already existing
   *  object in FTS; take directly the FtsMessage as argument.
   *  Used also in the message box.
   */

  static String makeDescription(int offset, FtsMessage msg)
  {
    boolean noNewLine = false;
    boolean addBlank = false;
    StringBuffer descr = new StringBuffer();

    for (int i = offset; i < msg.getNumberOfArguments(); i++)
      {
	Object value;

	if (addBlank)
	  descr.append(" ");
	else
	  addBlank = true;

	value = msg.getArgument(i);

	if (value instanceof Float)
	  {
	    descr.append(numberFormat.format(value));
	  }
	else
	  {
	    descr.append(value);

	    if (value.equals("$"))
	      addBlank = false;
	    else if (value.equals("'"))
	      {
		noNewLine = true;
		addBlank = false;
	      }
	    else if (value.equals(";"))
	      {
		if (noNewLine)
		  noNewLine = false;
		else
		  {
		    descr.append("\n");
		    addBlank = false;
		  }
	      }
	    else
	      noNewLine = false;
	  }
      }

    return descr.toString();
  }


  static FtsObject makeFtsObjectFromMessage(FtsMessage msg) throws FtsException
  {
    String className;
    StringBuffer description;
    FtsContainerObject parent;
    int objId;

    parent = (FtsContainerObject) msg.getArgument(0);
    objId = ((Integer) msg.getArgument(1)).intValue();

    /* Check for null description object */

    if (msg.getNumberOfArguments() < 3)
      return new FtsStandardObject(parent, "", "", objId);

    className = (String) msg.getArgument(2);

    if (className.equals("table"))
      return new FtsTableObject(parent, className, makeDescription(2, msg), objId);
    else if (className.equals("qlist"))
      return new FtsQlistObject(parent, className, makeDescription(2, msg), objId);
    else if (className.equals("patcher"))
      return new FtsPatcherObject(parent, makeDescription(3, msg), objId);
    else if (className.equals("inlet"))
      return new FtsInletObject(parent, ((Integer) msg.getArgument(3)).intValue(), objId);
    else if (className.equals("outlet"))
      return new FtsOutletObject(parent, ((Integer) msg.getArgument(3)).intValue(), objId);
    else if (className.equals("messbox"))
      return new FtsMessageObject(parent, makeDescription(3, msg), objId);
    else if (className.equals("comment"))
      return new FtsCommentObject(parent, makeDescription(3, msg), objId);
    else if (className.equals("slider"))
      return new FtsValueObject(parent, className, makeDescription(2, msg), objId);
    else if (className.equals("intbox"))
      return new FtsValueObject(parent, className, makeDescription(2, msg), objId);
    else if (className.equals("floatbox"))
      return new FtsValueObject(parent, className, makeDescription(2, msg), objId);
    else if (className.equals("toggle"))
      return new FtsValueObject(parent, className, makeDescription(2, msg), objId);
    else if (className.equals("param"))
      return new FtsValueObject(parent, className, makeDescription(2, msg), objId);
    else if (className.equals("__selection"))
      return new FtsSelection(parent, className, "__selection", objId);
    else if (className.equals("__clipboard"))
      return new FtsClipboard(parent, className, "__clipboard", objId);
    else
      return new FtsStandardObject(parent, className, makeDescription(2, msg), objId);
  }


  static FtsObject makeFtsAbstractionFromMessage(FtsMessage msg) throws FtsException
  {
    String className;
    StringBuffer description;
    FtsContainerObject parent;
    int objId;

    parent = (FtsContainerObject) msg.getArgument(0);
    objId = ((Integer) msg.getArgument(1)).intValue();
    className = (String) msg.getArgument(2);

    /* if the object has been succesfully created, set the parent dirty */

    return new FtsAbstractionObject(parent, className, makeDescription(2, msg), objId);
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
      Object owner;

      PropertyHandlerEntry(String name, FtsPropertyHandler handler, Object owner)
      {
	this.name    = name;
	this.handler = handler;
	this.owner = owner;
	table.addElement(this);
      }

      void callHandler(String name, Object value, Object author)
      {
	if (name.equals(this.name))
	  if ((author == null) || (author != owner))
	    handler.propertyChanged(FtsObject.this, name, value);
      }
    }

    public void removeWatch(Object owner)
    {
      // Shitty code; actually, the handler table should 
      // not be a vector ... may be a linked list

      Vector toRemove = new Vector();

      for (int i = 0; i < table.size(); i++)
	{
	  PropertyHandlerEntry ph = (PropertyHandlerEntry) table.elementAt(i);

	  if (ph.owner == owner)
	    toRemove.addElement(ph);
	}

      for (int i = 0; i < toRemove.size(); i++)
	table.removeElement((PropertyHandlerEntry)toRemove.elementAt(i));
    }

    public void removeWatch(String name, Object owner)
    {
      // Shitty code; actually, the handler table should 
      // not be a vector ... may be a linked list

      Vector toRemove = new Vector();

      for (int i = 0; i < table.size(); i++)
	{
	  PropertyHandlerEntry ph = (PropertyHandlerEntry) table.elementAt(i);

	  if ((ph.owner == owner) && (ph.name == name))
	    toRemove.addElement(ph);
	}

      for (int i = 0; i < toRemove.size(); i++)
	table.removeElement((PropertyHandlerEntry)toRemove.elementAt(i));
    }

    public void watch(String property, FtsPropertyHandler handler, Object owner)
    {
      new PropertyHandlerEntry(property, handler, owner);
    }

    synchronized void callHandlers(String name, Object value, Object author)
    {
      for (int i = 0; i < table.size(); i++)
	((PropertyHandlerEntry) table.elementAt(i)).callHandler(name, value, author);
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
    put(name, new Integer(value), null);
  }

  public void put(String name, float value)
  {
    put(name, new Float(value), null);
  }

  public void put(String name, Object value)
  {
    put(name, value, null);
  }

  public void put(String name, int value, Object author)
  {
    put(name, new Integer(value), author);
  }

  public void put(String name, float value, Object author)
  {
    put(name, new Float(value), author);
  }

  public void put(String name, Object value, Object author)
  {
    Object old;

    old = get(name);
    
    if ((old == null) || (! old.equals(value)))
      {
	if (FtsPropertyDescriptor.isPersistent(name))
	  setDirty();

	Fts.getServer().putObjectProperty(this, name, value);
	
	localPut(name, value, author);
      }
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
	x = ((Integer)value).intValue();
	return true;
      }
    else if (name.equals("y"))
      {
	y = ((Integer)value).intValue();
	return true;
      }
    else if (name.equals("w"))
      {
	width = ((Integer)value).intValue();
	return true;
      }
    else if (name.equals("h"))
      {
	height = ((Integer)value).intValue();
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
      return new Integer(x);
    else if (name.equals("y"))
      return new Integer(y);
    else if (name.equals("w"))
      return new Integer(width);
    else if (name.equals("h"))
      return new Integer(height);
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
    localPut(name, value, null);
  }

  void localPut(String name, Object value, Object author)
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
      propertyHandlerTable.callHandlers(name, value, author);

    // Call the handlers in the parent

    if (parent != null)
      parent.callWatchAll(name, value, author);
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

  public void watch(String property, FtsPropertyHandler handler)
  {
    watch(property, handler, handler);
  }

  public void watch(String property, FtsPropertyHandler handler, Object owner)
  {
    if (propertyHandlerTable == null)
      propertyHandlerTable = new PropertyHandlerTable();
    
    propertyHandlerTable.watch(property, handler, owner);
  }

  public void removeWatch(Object owner)
  {
    if (propertyHandlerTable != null)
      propertyHandlerTable.removeWatch(owner);
  }

  public void removeWatch(String name, Object owner)
  {
    if (propertyHandlerTable != null)
      propertyHandlerTable.removeWatch(name, owner);
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

  protected int x; 
  protected int y;
  protected int width;
  protected int height;

  /** Direct access to geometric properties; recomended when possible */
  /** They don't fire listener, they should !!! */

  public final int getX()
  {
    return x;
  }

  final void setX(int p)
  {
    x = p;
  }

  public final int getY()
  {
    return y;
  }

  final void setY(int p)
  {
    y = p;
  }

  public final int getWidth()
  {
    return width;
  }

  final void setWidth(int p)
  {
    width = p;
  }

  public final int getHeight()
  {
    return height;
  }

  final void setHeight(int p)
  {
    height = p;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object.
   */

  protected FtsObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super();

    this.className = className;
    this.description = description;
    this.parent = parent;

    if (objId != -1)
      setObjectId(objId);

    // this test make sense only for the root patcher

    if (parent != null)
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
    String name;

    name = this.getClass().getName();

    return "<" + name.substring(name.lastIndexOf('.') + 1) + " " + "{" + description + "}" + " #" + ftsId + ">";
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

  /** Get the MaxDocument this objects is part of */

  public MaxDocument getDocument()
  {
    if (parent != null)
      return parent.getDocument();
    else
      return null;
  }

  /** Tell the document this object is changed, 
   *  and do not represent the state of the original file (if any)
   *  anymore
   */

  public void setDirty()
  {
    MaxDocument document;

    document = getDocument();

    if (document != null)
      document.setSaved(false);
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
    Fts.getSelection().removeObject(this);
    parent.setDirty();
    parent.removeObjectFromContainer(this); 
    Fts.getServer().freeObject(this);
  }

  // Communication with the object

  /** Send a message to an object (in FTS). */

  public final void sendMessage(int inlet, String selector, Vector args)
  {
    Fts.getServer().sendObjectMessage(this, inlet, selector, args);
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

  final int getObjectId()
  {
    return ftsId;
  }

  /** Set the objid. Private, used only by the server. */

  final void setObjectId(int id)
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









