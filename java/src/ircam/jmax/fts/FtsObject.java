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

public class FtsObject 
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

  static public FtsObject makeFtsObject(FtsObject parent, String description)
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

  static public FtsObject makeFtsObject(FtsObject parent, String className, String argsDescr)
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

  static public FtsObject makeFtsObject(FtsObject parent, String className, Vector args)
  {
    // this is the real function makeFtsObject ; the other just preprocess arguments,
    // and then call this one; later, this function will select the actual class
    // to instantiate

    return new FtsObject(parent, className, args);
  }

  /**
   * Static function to build a FtsObject.<br>
   * Like makeFtsObject(FtsObject, String), but add a prebuilt graphic description
   * to the object.
   *
   * @see FtsObject#makeFtsObject(FtsObject, String)
   */

  static public FtsObject makeFtsObject(FtsObject parent, String descr, FtsGraphicDescription graphicDescr)
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

  static public FtsObject makeFtsObject(FtsObject parent, String className, Vector args,
					FtsGraphicDescription graphicDescr)
  {
    FtsObject obj;

    obj = makeFtsObject(parent, className, args);

    obj.setGraphicDescription(graphicDescr);

    return obj;
  }

  /**
   * Static function to build a FtsObject.<br>
   * Like makeFtsObject(FtsObject, String, FtsGraphicDescription), but add a declaration
   * flag to the argument; a declaration is an object that must be saved and instantiated
   * before the other objects.
   *
   * @see FtsObject#makeFtsObject(FtsObject, String, FtsGraphicDescription)
   */

  static public FtsObject  makeFtsObject(FtsObject parent, String descr,
					 FtsGraphicDescription graphicDescr, boolean declaration)
  {
    FtsObject obj;

    obj = makeFtsObject(parent, descr);

    obj.setGraphicDescription(graphicDescr);
    obj.setDeclaration(declaration);

    return obj;
  }

  /**
   * Static function to build a FtsObject.<br>
   * Like makeFtsObject(FtsObject, String, Vector, FtsGraphicDescription), but add a declaration
   * flag to the argument; a declaration is an object that must be saved and instantiated
   * before the other objects.
   *
   * @see FtsObject#makeFtsObject(FtsObject, String, Vector, FtsGraphicDescription)
   */

  static public FtsObject  makeFtsObject(FtsObject parent, String className, Vector args,
					 FtsGraphicDescription graphicDescr, boolean declaration)
  {
    FtsObject obj;

    obj = makeFtsObject(parent, className, args);

    obj.setGraphicDescription(graphicDescr);
    obj.setDeclaration(declaration);

    return obj;
  }


  /**
   * Static function to redefine a FtsObject.
   * It is static, for
   * similarity with the constructors, because it can produce 
   * a different Java object from the argument, so logically they are not method of the object.
   *
   * @param obj the object to redefine.
   * @param className the name of the FTS class of the object we want to instantiate.
   * @param args  a Vector containing the object arguments.
   * @return obj or a new object, conforming to the new definition, but with the same FTS
   * identity, and connections.
   */

  public static FtsObject redefineFtsObject(FtsObject obj, String className, Vector args)
  {
    obj.className = className;
    obj.args = args;

    MaxApplication.getFtsServer().redefineObject(obj, className, args);

    if (className.equals("patcher"))
      {
	obj.subPatcher.redefine(((Integer)args.elementAt(1)).intValue(),
				((Integer)args.elementAt(2)).intValue());
      }

    if (obj.parent.isOpen() && obj.updated)
      {	
	obj.getProperty("ninlets");
	obj.getProperty("noutlets");
	obj.getProperty("declaration");
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

  FtsObject parent;

  /** The Fts object class Name */

  private String className;	

  // sometimes, like in template and abstractions
  // the class name used by FTS differer from the official
  // className; by the way, this is an hack to be solved with
  // multiple classes.

  private String ftsClassName;	

  /** The vector containing the obejct argument */

  Vector args;

  /** The object subpatch. Null if the object is not a container. */

  FtsPatcher subPatcher = null;	

  /** The number of inlets of this object. Meaningfull only for "open" objects. */

  int ninlets;		    

  /** The number of outlets of this object. Meaningfull only for "open" objects. */

  int noutlets;

  /**
   * True if this object is a declaration.
   * Declaration objects must be loaded before the others in the same container.
   *
   * Note that a newly created declaration on the FTS side should send
   * the declaration property as soon as it known to be a declaration,
   * inside the "init" method; this field will be valid at the first
   * "sync" after the object creation.
   * The "declaration" property must be sent also if the window is
   * not open.
   */

  boolean declaration = false; 

  /** True is the object is an .abs abstraction */

  boolean abstraction = false; 

  /** True if it is a Tcl template. */

  boolean template = false; 

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



  /**
   * The empty constructor.
   * Used only locally to build the special
   * root object.
   */

  private FtsObject()
  {
  }

  /**
   * Create a FtsObject object.
   * Note that there is a small set of FTS primitive classes that are
   * known to the application layer, and require special handling, in 
   * particular "patcher", "inlet" and "outlet".
   */

  private FtsObject(FtsObject parent, String className, Vector args)
  {
    super();
    FtsPatcher patcher;


    this.className = className;
    this.ftsClassName = className; // by default
    this.args = args;
    this.parent = parent;

    patcher = parent.getSubPatcher();

    patcher.addObject(this);

    if (FtsTemplateTable.exists(className))
      {
	// A template is a tcl function that is executed to 
	// create the actual patcher content; the patcher is
	// passed as first argument, the user argument follow
	// the patcher.

	setTemplate(true);

	// If there is no description1, generate one now, from the
	// the arguments before the instantiation
	// the call is fancy, but it actually generate and cache 
	// a description if it does not exists; if it exists,
	// it is a no-op.
	
	description = getDescription(); 

	// Then, reset the ftsClassName to "patcher"

	this.ftsClassName = "patcher";

	//create a 0 in 0 out patcher FtsObject

	FtsObject obj;
	Vector oargs = new Vector();
	oargs.addElement("unnamed"); // we want to have the name fixed by the template !!!
	oargs.addElement(new Integer(0));
	oargs.addElement(new Integer(0));

	MaxApplication.getFtsServer().newObject(parent, this, "patcher", oargs);

	// load the patcher content from the file

	subPatcher = new FtsPatcher(this, "unnamed", 0, 0);

	// Should really do something better here in case of error !!!
	// raising exceptions ... 

	Interp interp  = MaxApplication.getTclInterp();

	try
	  {
	    // Call the tcl template function, with the container (this) as 
	    // first argument, and the other args following.

	    TclObject list = TclList.newInstance();

	    TclList.append(interp, list, TclString.newInstance(FtsTemplateTable.getProc(className)));
	    TclList.append(interp, list, ReflectObject.newInstance(interp, this));

	    for (int i = 1; i < args.size(); i++)
	      {
		Object arg = args.elementAt(i);

		if (arg instanceof Integer)
		  TclList.append(interp, list, TclInteger.newInstance(((Integer)arg).intValue()));
		else if (arg instanceof Float)
		  TclList.append(interp, list, TclDouble.newInstance(((Float)arg).doubleValue()));
		else if (arg instanceof String)
		  TclList.append(interp, list, TclString.newInstance((String) arg));
		else
		  TclList.append(interp, list, ReflectObject.newInstance(interp, arg));
	      }

	    interp.eval(list, 0);
	  }
 	catch (tcl.lang.TclException e)
	  {
	    System.out.println("TCL Error in template " + className + ":" + interp.getResult());
	  }

	loaded();	// activate the post-load init, like loadbangs
      }
    else if (className.endsWith(".pat") || className.endsWith(".abs") ||
	FtsAbstractionTable.exists(className))
      {
	// An abstraction is translated in its expansion (code from the FtsDotPat parser)
	// But, it is stored back as an object

	String patname;
	String realName;

	/* put the class name to "patcher"; the description will
	   not change */

	setAbstraction(true);

	// If there is no description, generate one now, from the
	// the arguments before the instantiation
	// the call is fancy, but it actually generate and cache 
	// a description if it does not exists; if it exists,
	// it is a no-op.
	
	description = getDescription(); 

	// Then, reset the ftsClassName to "patcher"

	this.ftsClassName = "patcher";

	// First, remove the .pat or the .abs if present, and
	// compute the real name

	if (className.endsWith(".pat"))
	  realName = className.substring(0, className.lastIndexOf(".pat"));
	else if (className.endsWith(".abs"))
	  realName = className.substring(0, className.lastIndexOf(".abs"));
	else
	  realName = className;



	if (FtsAbstractionTable.exists(realName))
	  {
	    patname = FtsAbstractionTable.getFilename(realName);
	  }
	else
	  patname = realName;

	//create a 0 in 0 out patcher FtsObject

	FtsObject obj;
	Vector oargs = new Vector();

	oargs.addElement("unnamed");
	oargs.addElement(new Integer(0));
	oargs.addElement(new Integer(0));

	MaxApplication.getFtsServer().newObject(parent, this, "patcher", oargs);

	// load the patcher content from the file

	subPatcher = new FtsPatcher(this, "unnamed", 0, 0);

	// Should really do something better here in case of error !!!
	// raising exceptions ... 

	try
	  {
	    FtsDotPatParser.importAbstraction(this, new File(patname), args);
	  }
	catch (FtsDotPatException e)
	  {
	    System.out.println("Error " + e + " in reading abstraction " + realName);
	  }
	catch (java.io.IOException e)
	  {
	    System.out.println("I/O Error " + e + " in reading abstraction " + realName);
	  }

	subPatcher.assignInOutletsAndName("unnamed");

	subPatcher.loaded();	// activate the post-load init, like loadbangs
      }
    else if (className.equals("patcher"))
      {
	subPatcher = new FtsPatcher(this, 
				    (String) args.elementAt(0),
				    ((Integer)args.elementAt(1)).intValue(),
				    ((Integer)args.elementAt(2)).intValue());

	MaxApplication.getFtsServer().newObject(parent, this, className, args);// create the fts object
      }
    else if (className.equals("inlet"))
      {
	if (args.size() >= 1)
	  patcher.addInlet(this, ((Integer)args.elementAt(0)).intValue());
	else
	  patcher.addInlet(this); // support for .pat

	MaxApplication.getFtsServer().newObject(parent, this, className, args);// create the fts object
      }
    else if (className.equals("outlet"))
      {
	if (args.size() >= 1)
	  patcher.addOutlet(this, ((Integer)args.elementAt(0)).intValue());
	else
	  patcher.addOutlet(this); // support for .pat

	MaxApplication.getFtsServer().newObject(parent, this, className, args);// create the fts object
      }
    else
      MaxApplication.getFtsServer().newObject(parent, this, className, args);// create the fts object

    if (parent.isOpen())
      {
	updated = true;
	getProperty("ninlets");
	getProperty("noutlets");

	MaxApplication.getFtsServer().syncToFts();
      }
  }


  /**
   * Build the root object.
   * The root object is the super patcher of everything;
   * cannot be edited, can include only patchers.
   */

  static FtsObject makeRootObject(FtsServer server)
  {
    FtsObject obj;

    // Build the arguments

    obj = new FtsObject();

    obj.className = "patcher";

    obj.args = new Vector();

    obj.args.addElement("root");
    obj.args.addElement(new Integer(0));
    obj.args.addElement(new Integer(0));

    obj.parent = null;
    obj.graphicDescr = null;

    // create it in FTS

    server.newObject(null, obj, "patcher", obj.args);

    // set the subpatcher (the root patcher)

    obj.subPatcher = new FtsPatcher(obj, "root", 0, 0);

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
    if (parent == null)
       return "FtsObject<" + className + ":" + ftsId +  ">";
     else if (isOpen())
       return "FtsObject<" + className + ":" + ftsId + " (" + ninlets + ":" + noutlets + ")>";
    else
      return "FtsObject<" + className + ":" + ftsId + ">";
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                      CLIENT API and  PROPERTIES                           */
  /*                                                                           */
  /*****************************************************************************/

  /* Accessors and selectors. */

  /**
   * Container property.
   * A container represent under some form
   * a subpatch, i.e. an object that can contains other objects.
   */

  public boolean isContainer()
  {
    return subPatcher != null;
  }

  /** Get the object including patcher. */

  public FtsObject getParent()
  {
    return parent;
   }

  /** Get the object class Name. */

  public String getClassName()
  {
    return className;
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
	    StringBuffer b;

	    b = new StringBuffer();

	    b.append(args.elementAt(0));

	    for (int i = 1; i < args.size(); i++)
	      {
		b.append(" ");
		b.append(args.elementAt(i).toString());
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

    if (ftsClassName.equals("patcher"))
      {
	subPatcher.redefine(((Integer)args.elementAt(1)).intValue(),
			    ((Integer)args.elementAt(2)).intValue());
      }

    if (isOpen() && updated)
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


  /** Get the subPatcher, an implementational object represeting a patcher content. */

  FtsPatcher getSubPatcher()
  {
    return subPatcher;
  }

  /** Set the subPatcher of an object,only for .pat support. */

  void setSubPatcher(FtsPatcher subPatcher)
  {
    this.subPatcher = subPatcher;

    subPatcher.setObject(this);
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


  /** Set the declaration property. */

  public void setDeclaration(boolean v)
  {
    declaration = v;
  }

  /** Get the declaration boolean property. */

  public boolean isDeclaration()
  {
    return declaration;
  }

  /** Set the abstraction property. */

  public void setAbstraction(boolean v)
  {
    abstraction = v;
  }

  /** Get the abstraction boolean property. */

  public boolean isAbstraction()
  {
    return abstraction;
  }


  /** Set the template property. */

  public void setTemplate(boolean v)
  {
    template = v;
  }

  /** Get the template boolean property. */

  public boolean isTemplate()
  {
    return template;
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                               SERVER COMMUNICATION                        */
  /*                                                                           */
  /*****************************************************************************/


  /**
   * Get the object id. <p>
   * Should *not* be public, the TCL IDs should be handled
   * separately, so to handle other kind of objects.
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
    else if (name.equals("declaration"))
      {
	setDeclaration(true);
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
  
  /** Save the object arguments to a TCL stream. */

  private void saveArgsAsTcl(FtsSaveStream stream)
  {
    stream.print("{");
    stream.print(getDescription());
    stream.print("}");
  }

  /** Save the object to a TCL stream. */

  void saveAsTcl(FtsSaveStream stream)
  {
    if (isDeclaration())
      {
	// Save as "declare ..."

	stream.print("declare $objs(" + parent.idx + ") ");

	saveArgsAsTcl(stream);

	stream.print(" ");

	if (graphicDescr != null)
	  graphicDescr.saveAsTcl(stream);
      }
    else if ((! isTemplate()) && (! isAbstraction()) && (subPatcher != null))
      {
	if (parent == MaxApplication.getFtsServer().getRootObject())
	  {
	    stream.print("patcher ");

	    if (subPatcher.windowDescr != null)
	      subPatcher.windowDescr.saveAsTcl(stream);

	    // This is a root patcher
	  }
	else
	  {
	    // Save as "patcher ..."

	    stream.print("patcher $objs(" + parent.idx + ") " +
			 (String) args.elementAt(0) + " " +
			 subPatcher.ninlets + " " + subPatcher.noutlets + " ");

	    if (graphicDescr != null)
	      graphicDescr.saveAsTcl(stream);

	    stream.print(" ");

	    if (subPatcher.windowDescr != null)
	      subPatcher.windowDescr.saveAsTcl(stream);
	  }
      }
    else
      {
	// Save as "object ..."

	stream.print("object $objs(" + parent.idx + ") ");

	saveArgsAsTcl(stream);

	stream.print(" ");

	if (graphicDescr != null)
	  graphicDescr.saveAsTcl(stream);
      }
  }

  /**
   * Tell Fts the patcher is Open.
   * An open patcher is a patcher for which we want
   * continuous updates.
   */

  public void open()
  {
    if (subPatcher != null)
      subPatcher.open();
  }

  /**
   * Tell Fts the patcher is Closed.
   * An open patcher is a patcher for which we want
   * continuous updates.
   */

  public void close()
  {
    if (subPatcher != null)
      subPatcher.close();
  }

  /** Check if the object is an open patcher. */

  public boolean isOpen()
  {
    if (subPatcher != null)
      return subPatcher.isOpen();
    else
      return false;
  }


  /**
   * Declare the patcher loaded.
   * This fire the after load initializations
   * in FTS.
   */

  public void loaded()
  {
    if (subPatcher != null)
      subPatcher.loaded();
  }

  /**
   * Return the contained objects.
   * For an object representing a patcher, return
   * a Vector of all the FtsObject in the patcher.
   */

  public Vector getContainedObjects()
  {
    if (subPatcher != null)
      return subPatcher.getObjects();
    else
      return null;
  }

  /**
   * Return the contained connections.
   * For an object representing a patcher, return
   * a Vector of all the FtsConnection in the patcher.
   */

  public Vector getContainedConnections()
  {
    if (subPatcher != null)
      return subPatcher.getConnections();
    else
      return null;
  }

  /** Get the Window description of a patcher. */

  public FtsWindowDescription getWindowDescription()
  {
    if (subPatcher != null)
      return subPatcher.getWindowDescription();
    else
      return null;
  }

  /** Set the Window description of a patcher. */

  public void setWindowDescription(FtsWindowDescription wd)
  {
    if (subPatcher != null)
      subPatcher.setWindowDescription(wd);
  }


  /**
   * Save the object to an output stream.
   * <i>Bug: it actually work only if the object is a patcher.</i>
   */

  public void saveTo(OutputStream stream)
  {
    if (subPatcher != null)
      subPatcher.saveTo(stream);
  }


  /**
   * Find all the objects that have a given pattern 
   * in its descriptions.
   */

  public Vector find(String pattern)
  {
    if (subPatcher != null)
      {
	Vector v = new Vector();

	subPatcher.find(pattern, v);

	return v;
      }
    else
      return null;
  }

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






