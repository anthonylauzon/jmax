package ircam.jmax.fts;

import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

abstract public class FtsObject 
{
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

    // Add check for declarations, when they exists,
    // comments and message box when description and argument description
    // are merged

    // The check on patcher here is temporary; patchers will move
    // to a specific API as soon as the patcher object is ready.

    if (className.equals("patcher"))
      return new FtsPatcherObject(parent, description);
    else if (className.equals("inlet"))
      throw new FtsException(new FtsError(FtsError.FTS_INSTANTIATION_ERROR, "inlet"));
    else if (className.equals("outlet"))
      throw new FtsException(new FtsError(FtsError.FTS_INSTANTIATION_ERROR, "outlet"));
    else if (className.equals("message"))
      throw new FtsException(new FtsError(FtsError.FTS_INSTANTIATION_ERROR, "message"));
    else if (className.equals("comment"))
      throw new FtsException(new FtsError(FtsError.FTS_INSTANTIATION_ERROR, "comment"));
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
   * You cannot redefine a message, comment, inlet and outlet object.
   *
   * DO NOT WORK: MORE WORK TO BE DONE, otherwise is not compatible with abstractions and templates,
   * and multiclass; the object must always be rebuilt and substituted in the connections.
   * @param obj the object to redefine.
   * @param className the name of the FTS class of the object we want to instantiate.
   * @param description  a Vector containing the description.
   * @return obj or a new object, conforming to the new definition, but with the same FTS
   * identity, and connections.
   */

  public static FtsObject redefineFtsObject(FtsObject obj, String description)
  {
    // To be completely rewritten 

    return obj;
  }


  /******************************************************************************/
  /*                                                                            */
  /*              Object Variables                                              */
  /*                                                                            */
  /******************************************************************************/

  /** Fts Object ID  */

  private int ftsId;

  /** The parent object. Usually the container patcher or abstraction or template */

  FtsContainerObject parent;

  /** The Fts class Name */

  String className;	

  /** The object name, if exists */

  String name;

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

  FtsPropertyHandlerTable propertyTable = null;

  /** The message Handler Table for this object */

  Vector messageTable = null;

  /** The graphic description for this object, if any. */

  FtsGraphicDescription graphicDescription = null;

  /** on screen representation of the Fts Object */

  Object representation;

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

    parent.addObject(this);
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

  /** Get the name */

  final public String getName()
  {
    return name;
  }

  /** Set the name */

  final void setName(String name)
  {
    this.name = name;
  }

  /** Get the complete textual description of the object. */

  public String getDescription()
  {
    return description;
  }

  /** Get the number of inlets of the object */

  public final int getNumberOfInlets()
  {
    return ninlets;
  }

  /** Get the number of outlets of the object */

  public final int getNumberOfOutlets()
  {
    return noutlets;
  }

  /**
   * Delete the object from fts. 
   * Fts object should be delete explicitely, the finalizer will not delete them.
   */

  public void delete()
  {
    parent.removeObject(this); 
    MaxApplication.getFtsServer().freeObject(this);
  }

  // Communication with the object

  /** Send a message to an object (in FTS). */

  public final void sendMessage(int inlet, String selector, Vector args)
  {
    MaxApplication.getFtsServer().sendObjectMessage(this, inlet, selector, args);
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
      messageTable.removeElement(handler);
  }


  // Property handling

  /**
   * Called by the application to change an object property.
   */

  public void putProperty(String name, Object value)
  {
    // Should check for local properties in the subclasses

    MaxApplication.getFtsServer().putObjectProperty(this, name, value);
  }

  /**
   * Ask fts to send back the current value of a property.
   * The value will be delivered thru the installed property handler.
   *
   * @see FtsObject#installPropertyHandler
   * @see FtsPropertyHandler
   */

  public void getProperty(String name)
  {
    MaxApplication.getFtsServer().getObjectProperty(this, name);
  }


  /** Install a property handler. */

  public void installPropertyHandler(String property, FtsPropertyHandler handler)
  {
    if (propertyTable == null)
      propertyTable = new FtsPropertyHandlerTable();

    propertyTable.addPropertyHandler(property, handler);
  }

  /** Remove a property handler. */

  public void removePropertyHandler(String property, FtsPropertyHandler handler)
  {
    if (propertyTable != null)
      propertyTable.removePropertyHandler(property, handler);
  }

  /**
   * Get the graphic description of the object.
   * The graphic description is
   * stored here by the editor, or a parser.
   */

  public final  FtsGraphicDescription getGraphicDescription()
  {
    return graphicDescription;
  }

  /**
   * Set the graphic description of the object.
   * The graphic description is stored here by the editor 
   * or a parser.
   */

  public final void setGraphicDescription(FtsGraphicDescription g)
  {
    graphicDescription = g;
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

  /**
    Called by the FTS server when a FTS object change a property value.
    Invoke the correct property dispatcher.

    It handle directly the object properties (ninlets and noutlets)
    and call a propertyChanged handler.
   */

  void serverSetProperty(String name, Object value)
  {
    // local properties

    if (name.equals("ninlets") && value instanceof Integer)
      {
	ninlets = ((Integer)value).intValue();
      }
    else if (name.equals("noutlets") && value instanceof Integer)
      {
	noutlets = ((Integer)value).intValue();
      }

    // calling handlers 

    if (propertyTable != null)
      propertyTable.callHandlers(name, value);
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

  /** Save the object properties as tcl code; should not print any new line;
   *  this function save the basic properties; other objects that want to save
   *  other properties should define a specialization, and call *super* in the code.
   */

  void savePropertiesAsTcl(PrintWriter writer)
  {
  }
}






