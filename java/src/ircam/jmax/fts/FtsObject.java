package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.mda.*;


/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

abstract public class FtsObject 
{
  /* code to set generic properties meta-properties */

  /******************************************************************************/
  /*                                                                            */
  /*              STATIC FUNCTION                                               */
  /*                                                                            */
  /******************************************************************************/

  /** This version create an application layer object for an already existing
   *  object in FTS; take directly the FtsMessage as argument.
   *  Used also in the message box.
   */

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

    className = ((String) msg.getArgument(2)).intern();

    if (className == "jpatcher")
      return new FtsPatcherObject(parent, FtsParse.unparseObjectDescription(3, msg), objId);
    else if (className == "inlet")
      return new FtsInletObject(parent, ((Integer) msg.getArgument(3)).intValue(), objId);
    else if (className == "outlet")
      return new FtsOutletObject(parent, ((Integer) msg.getArgument(3)).intValue(), objId);
    else if (className == "messbox")
      return new FtsMessageObject(parent, FtsParse.unparseObjectDescription(3, msg), objId);
    else if (className == "comment")
      return new FtsCommentObject(parent, FtsParse.simpleUnparseObjectDescription(3, msg), objId);
    else if (className == "__selection")
      return new FtsSelection(parent, className, "__selection", objId);
    else if (className == "__clipboard")
      return new FtsClipboard(parent, className, "__clipboard", objId);
    else
      return new FtsStandardObject(parent, className, FtsParse.unparseObjectDescription(2, msg), objId);
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

    return new FtsAbstractionObject(parent, className, FtsParse.unparseObjectDescription(2, msg), objId);
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
    MaxVector table = new MaxVector();

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
	if (name == this.name)
	  if ((author == null) || (author != owner))
	    handler.propertyChanged(FtsObject.this, name, value);
      }

      public String toString()
      {
	return "PropertyHandlerEntry for " + name + " handler " + handler + " owner " + owner;
      }
    }

    public void removeWatch(Object owner)
    {
      // Shitty code; actually, the handler table should 
      // not be a vector ... may be a linked list

      MaxVector toRemove = new MaxVector();

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

      MaxVector toRemove = new MaxVector();

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
   * "put" send the property to fts; localPut 
   * store and access the properties, activating
   * watchers here and in the container if needed.
   * Also "catch" and locally process properties
   * valid for all the objects.
   *
   *
   * Values here have been already parsed from the Tcl
   * command.
   * 
   */

  MaxVector properties = null;

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
    Fts.getServer().putObjectProperty(this, name, value);
  }


  /* ask fts to send back the value property (and so indirectly call the handlers) */
     
  public void ask(String name)
  {
    Fts.getServer().askObjectProperty(this, name);
  }
  
  /** Local put is a version of put that do not send
    values to FTS.
    */

  public void localPut(String name, int value)
  {
    localPut(name, new Integer(value), null);
  }

  public void localPut(String name, float value)
  {
    localPut(name, new Float(value), null);
  }

  public void localPut(String name, Object value)
  {
    localPut(name, value, null);
  }

  public void localPut(String name, int value, Object author)
  {
    localPut(name, new Integer(value), author);
  }

  public void localPut(String name, float value, Object author)
  {
    localPut(name, new Float(value), author);
  }


  void localPut(String name, Object value, Object author)
  {
    // check first hardcoded properties

    if (name == "ins")
      {
	if (! (value instanceof FtsVoid))
	  this.ninlets = ((Integer)value).intValue();
      }
    else if (name == "outs")
      {
	if (! (value instanceof FtsVoid))
	  this.noutlets = ((Integer)value).intValue();
      }
    else if (name == "x")
      {
	if (! (value instanceof FtsVoid))
	  x = ((Integer)value).intValue();
      }
    else if (name == "y")
      {
	if (! (value instanceof FtsVoid))
	  y = ((Integer)value).intValue();
      }
    else if (name == "w")
      {
	if (! (value instanceof FtsVoid))
	  width = ((Integer)value).intValue();
      }
    else if (name == "h")
      {
	if (! (value instanceof FtsVoid))
	  height = ((Integer)value).intValue();
      }
    else if (name == "name")
      {
	if (! (value instanceof FtsVoid))
	  setObjectName(value.toString());
      }
    else
      {
	// local properties

	if (properties == null)
	  properties = new MaxVector();

	search :
	  {
	    for (int i = 0; i < properties.size(); i++)
	      {
		Property p = (Property)(properties.elementAt(i));

		if (p.name == name)
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

  /** Get a propery value; subclasses may specialize it */

  public Object get(String name)
  {
    Object v = null;

    if (name == "ins")
      return new Integer(getNumberOfInlets());
    else if (name == "outs")
      return new Integer(getNumberOfOutlets());
    else if (name == "x")
      return new Integer(x);
    else if (name == "y")
      return new Integer(y);
    else if (name == "w")
      return new Integer(width);
    else if (name == "h")
      return new Integer(height);
    else if (name == "name")
      return getObjectName();

    if (properties != null)
      {
      search: for (int i = 0; i < properties.size(); i++)
	{
	  Property p = (Property)(properties.elementAt(i));
	    
	  if (p.name == name)
	    {
	      v = p.value;
	      break search;
	    }
	}
      }
    else
      v = FtsPropertyDescriptor.getDefaultValue(name);

    if (v == null)
      return FtsVoid.voidValue;
    else
      return v;
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


  /** x, y, width and height cache locally the geometrical properties, to speed
    up access; also, to prepare transition to beans model */

  protected int x = -1; 
  protected int y = -1 ;
  protected int width = -1;
  protected int height = -1;

  /** Direct access to geometric properties; recomended when possible;
   *  availables only on access.
   */

  public final int getX()
  {
    return x;
  }

  public final int getY()
  {
    return y;
  }

  public final int getWidth()
  {
    return width;
  }

  public final int getHeight()
  {
    return height;
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
  /*                               SERVICE LOCA LFUNCTIONS                     */
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
   * Ask FTS to Delete the object. 
   * Fts object should be delete explicitely, the finalizer will not delete them.
   * The actual deleting is done on the FTS delete callback, that call release.
   */

  public void delete()
  {
    Fts.getSelection().removeObject(this);
    parent.setDirty();
    Fts.getServer().deleteObject(this);
  }

  /**
   * Delete the Java object, without touching the FTS object represented.
   * 
   */

  public void release()
  {
    Fts.getSelection().removeObject(this);
    parent.setDirty();

    parent.removeObjectFromContainer(this); 

    // clean up to help the gc, and make the object
    // non functioning, so to catch use of the object
    // after the release/delete.

    propertyHandlerTable = null;
    properties = null;
    parent = null;
    className = null;
    objectName = null;
    description = null;

    Fts.getServer().unregisterObject(this);
  }

  // Communication with the object

  /** Send a message to an object (in FTS). */

  public final void sendMessage(int inlet, String selector, MaxVector args)
  {
    Fts.getServer().sendObjectMessage(this, inlet, selector, args);
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
   * Empty by default, subclassed by special objects
   */

  void handleMessage(FtsMessage msg)
  {
  }
}









