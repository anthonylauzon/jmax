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
   * It is a static method,  and
   * not constructor, because the FtsObject created may be of different
   * classes depending on the content of the object.
   *
   * @param parent the parent object. 
   * @param description  a string description of the object content, like
   * the content of an object box.
   */

  static public FtsObject makeFtsObject(FtsContainerObject parent, String description)
  {
    FtsObject obj;
    String className;
    Vector args;

    args = new Vector();
    
    className = FtsParse.parseObject(description, args);

    obj = FtsObject.makeFtsObject(parent, className, args);

    obj.description = description;	// store in the object its initial description
    
    return obj;
  }


  /**
   * Static function to build a FtsObject.
   * It is a static method, and
   * not constructor, because the FtsObject created may be of different
   * classes depending on the content of the object.
   *
   * @param parent the parent object.
   * @param className the name of the FTS class of the object we want to instantiate.
   * @param argsDescr  a string description of the object arguments, like
   * the content of an message box.
   */

  static public FtsObject makeFtsObject(FtsContainerObject parent, String className, String argsDescr)
  {
    FtsObject obj;
    Vector args;
    
    args = new Vector();
    
    FtsParse.parseObjectArgs(argsDescr, args);

    obj = FtsObject.makeFtsObject(parent, className, args);

    obj.argsDescription = argsDescr;	// store in the object its argument description

    return obj;
  }

  /**
   * Static function to build a FtsObject.
   * It is a static method, and
   * not constructor, because the FtsObject created may be of different
   * classes depending on the content of the object.
   *
   * @param parent the parent object.
   * @param className the name of the FTS class of the object we want to instantiate.
   * @param args  a Vector containing the object arguments.
   */

  static public FtsObject makeFtsObject(FtsContainerObject parent, String className, Vector args)
  {
    // Add check for declarations, when they exists,
    // comments and message box when description and argument description
    // are merged

    if (className.equals("patcher"))
      return new FtsPatcherObject(parent, args);
    else if (className.equals("inlet"))
      return new FtsInletObject(parent, args);
    else if (className.equals("outlet"))
      return new FtsOutletObject(parent, args);
    else if (className.endsWith(".pat") || className.endsWith(".abs") || FtsAbstractionTable.exists(className))
      return new FtsAbstractionObject(parent, className, args);
    else if (FtsTemplateTable.exists(className))
      return new FtsTemplateObject(parent, className, args);
    else
      return new FtsStandardObject(parent, className, args);
  }

  /**
   * Static function to build a FtsObject.<br>
   * Like makeFtsObject(FtsObject, String), but add a prebuilt graphic description
   * to the object.
   *
   * @see FtsObject#makeFtsObject(FtsObject, String)
   */

  static public FtsObject makeFtsObject(FtsContainerObject parent, String descr,
					FtsGraphicDescription graphicDescr)
  {
    FtsObject obj;

    obj = makeFtsObject(parent, descr);

    obj.setGraphicDescription(graphicDescr);

    return obj;
  }

  /**
   * Static function to build a FtsObject.<br>
   * Like makeFtsObject(FtsObject, String, Vector), but add a prebuilt graphic description
   * to the object.
   *
   * @see FtsObject#makeFtsObject(FtsObject, String, Vector)
   */

  static public FtsObject makeFtsObject(FtsContainerObject parent, String className, Vector args,
					FtsGraphicDescription graphicDescr)
  {
    FtsObject obj;

    obj = makeFtsObject(parent, className, args);

    obj.setGraphicDescription(graphicDescr);

    return obj;
  }


  /**
   * Static function to redefine a FtsObject.
   * It is static, for
   * similarity with the constructors, because it can produce 
   * a different Java object from the argument, so logically they are not method of the object.
   *
   * DO NOT WORK: MORE WORK TO BE DONE, otherwise is not compatible with abstractions and templates,
   * and multiclass; the object must always be rebuilt and substituted in the connections.
   * @param obj the object to redefine.
   * @param className the name of the FTS class of the object we want to instantiate.
   * @param args  a Vector containing the object arguments.
   * @return obj or a new object, conforming to the new definition, but with the same FTS
   * identity, and connections.
   */

  public static FtsObject redefineFtsObject(FtsObject argObj, String className, Vector args)
  {
    FtsStandardObject obj = (FtsStandardObject) argObj;

    obj.className = className;
    obj.args = args;

    MaxApplication.getFtsServer().redefineObject(obj, className, args);

    if (obj.parent.isOpen() && obj.updated)
      {	
	obj.getProperty("ninlets");
	obj.getProperty("noutlets");
	MaxApplication.getFtsServer().syncToFts();
      }

    return obj;
  }


  /**
   * Static function to redefine a FtsObject.
   * It is  static, for
   * similarity with the constructors, because it can produce 
   * a different Java object from the argument, so logically they are not method of the object.
   *
   * @param obj the object to redefine.
   * @param className the name of the FTS class of the object we want to instantiate.
   * @param argsDescr  a string description of the object arguments, like
   * the content of an message box.
   * @return obj or a new object, conforming to the new definition, but with the same FTS
   * identity, and connections.
   */

  public static FtsObject redefineFtsObject(FtsObject obj, String className, String argsDescr)
  {
    Vector args;
    FtsObject newobj;

    args = new Vector();

    FtsParse.parseObjectArgs(argsDescr, args);

    newobj = redefineFtsObject(obj, className, args);
    newobj.argsDescription = argsDescr;

    return newobj;
  }


  /**
   * Static function to redefine a FtsObject.
   * It is  static, for
   * similarity with the constructors, because it can produce 
   * a different Java object from the argument, so logically they are not method of the object.
   *
   * @param obj the object to redefine.
   * @param description  a string description of the object content, like
   * the content of an object box.
   * @return obj or a new object, conforming to the new definition, but with the same FTS
   * identity, and connections.
   */

  public static FtsObject redefineFtsObject(FtsObject obj, String description)
  {
    String className;
    Vector args;
    FtsObject newobj;
    
    args = new Vector();

    className = FtsParse.parseObject(description, args);

    newobj = redefineFtsObject(obj, className, args);
    newobj.description = description;

    return newobj;
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

  /** The Fts object class Name */

  private String className;	

  // sometimes, like in template and abstractions
  // the class name used by FTS differer from the official
  // className; by the way, this is an hack to be solved with
  // multiple classes.

  protected String ftsClassName;	

  /** The vector containing the obejct argument */

  Vector args;

  /** The number of inlets of this object. Meaningfull only for "open" objects and patchers */

  int ninlets;		    

  /** The number of outlets of this object. Meaningfull only for "open" objects and patchers */

  int noutlets;

  /**
   * True when the object is ready and kept consistent.
   * In an open patcher, don't try to keep it consistent if not ready (sync are expensive !!)
   */

  boolean updated = false;

  /**
   * The object string description. 
   * Usually set at object creation, but generated by need
   * for object created without using a string description.
   */

  String description = null;

  /**
   * The object string argument description. 
   * Usually set at object creation, but generated by need
   * for object created without using a string argument description.
   */

  String argsDescription = null;

  // Property and message handlers (to be converted to Beans style)
  // Should go in a structure create by need

  /** The property Handler Table for this object */

  FtsPropertyHandlerTable propertyTable = null;

  /** The message Handler Table for this object */

  Vector messageTable = null;

  /** The graphic description for this object, if any. */

  FtsGraphicDescription graphicDescr = null;

  /** on screen representation of the Fts Object */

  Object representation;

  /** Support for saving. Used to count indexes for the objs tcl array.*/

  int idx;

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
   * Note that there is a small set of FTS primitive classes that are
   * known to the application layer, and require special handling, in 
   * particular "patcher", "inlet" and "outlet".
   */

  protected FtsObject(FtsContainerObject parent, String className, Vector args)
  {
    super();

    this.className = className;
    this.ftsClassName = className; // by default
    this.args = args;
    this.parent = parent;

    parent.addObject(this);
  }


  /**
   * Build the root object.
   * The root object is the super patcher of everything;
   * cannot be edited, can include only patchers.
   */

  // Should go to the patcher object

  static FtsContainerObject makeRootObject(FtsServer server)
  {
    FtsPatcherObject obj;

    // Build the arguments

    obj = new FtsPatcherObject();

    obj.className = "patcher";
    obj.ftsClassName = "patcher";

    obj.args = new Vector();

    obj.args.addElement("root");
    obj.args.addElement(new Integer(0));
    obj.args.addElement(new Integer(0));

    obj.parent = null;
    obj.graphicDescr = null;

    // create it in FTS

    server.newObject(null, obj, obj.args);

    // set the subpatcher (the root patcher)

    obj.setSubPatcher(new FtsPatcher(obj, "root", 0, 0));

    // set the inlets and ooutlets.

    obj.ninlets = 0;
    obj.noutlets = 0;

    return obj;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               SERVICE LOCAL FUNCTIONS                     */
  /*                                                                           */
  /*****************************************************************************/

  /** Give a string representation of an object. */

  public String toString()
  {
    return "FtsObject:" + ftsId + "{" + getDescription() + "}";
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/

  /* Accessors and selectors. */

  /** Get the object including patcher. */

  public FtsContainerObject getParent()
  {
    return parent;
   }

  /** Get the object class Name. */

  public String getClassName()
  {
    return className;
  }

  /** Get the real object class Name */
  
  String getFtsClassName()
  {
    return ftsClassName;
  }

  /** Get the complete textual description of the object. */

  public String getDescription()
  {
    if (description != null)
      return description;
    else
      return className + " " + getArgumentsDescription();
  }

  
  /** Get the object arguments. */

  public Vector getArguments()
  {
    return args;
  }

  /**
   * Get the argument description.
   *
   * @return a string representation of the arguments.
   */

  public String getArgumentsDescription()
  {
    if (argsDescription != null)
      {
	return argsDescription;
      }
    else if (description != null)
      {
	return description.substring(description.indexOf(' '));
      }
    else
      {
	// rebuild the argument description

	if ((args == null) || (args.size() == 0))
	  return "";
	else if (args.size() == 1)
	  return args.elementAt(0).toString();
	else
	  {
	    Object element;
	    StringBuffer b;

	    b = new StringBuffer();

	    element = args.elementAt(0);
	    b.append(element.toString());
	  
	    for (int i = 1; i < args.size(); i++)
	      {
		// If we are generating the description of a message
		// box, follow the convention of Max 0.26 about new lines.
		// otherwise, add a blank between values

		if ((className == "message") &&
		    (element instanceof String) &&
		    (((String) element).equals(";")))
		  b.append("\n");
		else
		  b.append(" ");

		element = args.elementAt(i);
		b.append(element.toString());
	      }

	    return b.toString();
	  }
      }
  }


  /**
   * Set the arguments.
   * This may require an object redefinition
   * on the FTS side; this may then imply some comunication with FTS.
   */

  public void setArguments(Vector args)
  {
    this.args = args;

    // Use fts class name, not user class name

    MaxApplication.getFtsServer().redefineObject(this, ftsClassName, args);

    if (parent.isOpen() && updated)
      {
	getProperty("ninlets");
	getProperty("noutlets");
	MaxApplication.getFtsServer().syncToFts();
      }
  }


  /**
   * Set the arguments.
   * Set the argument passed as a string description; argument 
   * are parsed and the object redefined, as in setArguments.
   */

  public void setArgumentsDescription(String argsDescr)
  {
    argsDescription = argsDescr;

    args = new Vector();
    
    FtsParse.parseObjectArgs(argsDescr, args);

    setArguments(args);
  }


  /** Get the number of inlets of the object (valid only if the patcher is open). */

  public int getNumberOfInlets()
  {
    return ninlets;
  }

  /** Get the number of outlets of the object (valid only if the patcher is open). */

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
    // should tell the patcher to delete any left input connection
    // should tell the patcher delete any left output connection

    parent.getSubPatcher().removeObject(this); 
    MaxApplication.getFtsServer().freeObject(this);
  }

  // Communication with the object

  /** Send a message to an object (in FTS). */

  public void sendMessage(int inlet, String selector, Vector args)
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

  public FtsGraphicDescription getGraphicDescription()
  {
    return graphicDescr;
  }

  /**
   * Set the graphic description of the object.
   * The graphic description is stored here by the editor 
   * or a parser.
   */

  public void setGraphicDescription(FtsGraphicDescription g)
  {
    graphicDescr = g;
  }

  /** Get the representation of this object in the editor. */

  public Object getRepresentation()
  {
    return representation;
  }

  /** Set the representation of this object in the editor. */

  public void setRepresentation(Object r)
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

  int getObjId()
  {
    return ftsId;
  }

  /** Set the objid. Private, used only by the server. */

  void setObjId(int id)
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

  /** Save the object arguments to a TCL stream. */

  protected void saveArgsAsTcl(FtsSaveStream stream)
  {
    stream.print("{");
    stream.print(getDescription());
    stream.print("}");
  }

  /** Save the object to a TCL stream; actually defined by the object */

  abstract void saveAsTcl(FtsSaveStream stream);

  /** Access to the help patch for an object. */

  public File getHelpPatch()
  {
    if (FtsHelpPatchTable.exists(className))
      return new File(FtsHelpPatchTable.getHelpPatch(className));
    else
      return null;
  }

  /** Access to the html documentation for an object. */

  // Should return an URL object

  public String getReferenceURL()
  {
    if (FtsReferenceURLTable.exists(className))
      return FtsReferenceURLTable.getReferenceURL(className);
    else
      return null;
  }
}






