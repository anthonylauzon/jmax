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

public class FtsObject 
{

  /******************************************************************************/
  /*                                                                            */
  /*              STATIC FUNCTION                                               */
  /*                                                                            */
  /******************************************************************************/

  /** This version create an application layer object for an already existing
   *  object in FTS; take directly the FtsStream as argument.
   *  Used also in the message box.
   */

  static FtsObject makeFtsObjectFromMessage(FtsStream stream, boolean doVariable)
     throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException, FtsException
  {
    FtsPatcherData data;
    FtsObject parent;
    FtsObject obj;
    String variable = null;
    String className;
    StringBuffer description;
    int objId;

    parent = stream.getNextObjectArgument();
    data   = (FtsPatcherData) stream.getNextDataArgument();
    objId  = stream.getNextIntArgument();

    if (doVariable)
      variable =  stream.getNextStringArgument();
      
    /* Check for null description object */

    if (stream.endOfArguments())
      return new FtsObject(parent, "", null, "", objId);

    /* Get the class name */

    if (stream.nextIsSymbol())
      className = stream.getNextSymbolArgument();
    else
      className = "";

    /* Note that we do the unparsing relative to ':' and variables
       here; in the future, a dedicated API should be used ! */

    if (className == "jpatcher")
      {
	if (doVariable)
	  obj =  new FtsPatcherObject(parent, variable,
				      variable + " : " + FtsParse.unparseObjectDescription(stream), objId);
	else
	  obj =  new FtsPatcherObject(parent, variable,
				      FtsParse.unparseObjectDescription(stream), objId);
      }
    else if (className == "inlet")
      obj =  new FtsInletObject(parent, stream.getNextIntArgument(), objId);
    else if (className == "outlet")
      obj =  new FtsOutletObject(parent, stream.getNextIntArgument(), objId);
    else if (className == "messbox")
      obj =  new FtsMessageObject(parent, FtsParse.unparseObjectDescription(stream), objId);
    else if (className == "comment")
      obj =  new FtsCommentObject(parent, FtsParse.simpleUnparseObjectDescription(stream), objId);
    else if (className == "intbox")
      obj =  new FtsIntValueObject(parent, className, "intbox", objId);
    else if (className == "toggle")
      obj =  new FtsIntValueObject(parent, className, "toggle", objId);
    else if (className == "button")
      obj =  new FtsIntValueObject(parent, className, "button", objId);
    else if (className == "slider")
      obj =  new FtsSliderObject(parent, "slider", objId);
    else if (className == "floatbox")
      obj =  new FtsFloatValueObject(parent, className, "floatbox", objId);
    else if (className == "__selection")
      obj =  new FtsSelection(parent, className, "__selection", objId);
    else if (className == "__clipboard")
      obj =  new FtsClipboard(parent, className, "__clipboard", objId);
    else
      {
	if (doVariable)
	  obj = new FtsObject(parent, className, variable,
			      variable + " : " + FtsParse.unparseObjectDescription(className, stream), objId);
	else
	  obj = new FtsObject(parent, className, variable,
			      FtsParse.unparseObjectDescription(className, stream), objId);
      }

    if (data != null)
      data.addObject(obj);

    return obj;
  }

  /******************************************************************************/
  /*                                                                            */
  /*              The property system                                           */
  /*                                                                            */
  /******************************************************************************/


  /** Local put is a version of put that do not send
    values to FTS.
    */

  protected void localPut(String name, int value)
  {
    if (name == "ins")
      {
	ninlets = value;

	if ((data != null) && (data instanceof FtsPatcherData))
	  ((FtsPatcherData) data).firePatcherChangedNumberOfInlets(ninlets);

	if (listener instanceof FtsInletsListener)
	  ((FtsInletsListener)listener).inletsChanged(ninlets);
      }
    else if (name == "outs")
      {
	noutlets = value;

	if ((data != null) && (data instanceof FtsPatcherData))
	  ((FtsPatcherData) data).firePatcherChangedNumberOfOutlets(noutlets);

	if (listener instanceof FtsOutletsListener)
	  ((FtsOutletsListener)listener).outletsChanged(noutlets);
      }
    else if (name == "x")
      {
	x = value;
      }
    else if (name == "y")
      {
	y = value;
      }
    else if (name == "w")
      {
	width = value;
      }
    else if (name == "h")
      {
	height = value;
      }
    else if (name == "error")
      {
	if (value == 0)
	  isError = false;
	else
	  isError = true;

	if (listener instanceof FtsObjectErrorListener)
	  ((FtsObjectErrorListener)listener).errorChanged(isError);
      }
    else if (name == "fs")
      {
	fontSize = value;
      }
  }

  protected void localPut(String name, float value)
  {
    // No float property at this level
  }


  protected void localPut(String name, Object value)
  {
    // check first hardcoded properties

    if (name == "errdesc")
      {
	errorDescription = (String) value;
      }
    else if (name == "font")
      {
	font = (String) value;
      }
    else if (name == "name")
      {
	setObjectName((String) value);
      }
    else if (name == "data")
      {
	data = (MaxData) value;
	Fts.fireNewDataListenerOn(this, data);
      }
  }


  // New simplified beanified system

  protected Object listener;

  public void setObjectListener(Object obj)
  {
    listener = obj;
  }

  /******************************************************************************/
  /*                                                                            */
  /*              Object Variables                                              */
  /*                                                                            */
  /******************************************************************************/

  /** Fts Object ID  */

  private int ftsId = -1;

  /** The parent object. Usually the container patcher or abstraction or template */

  FtsObject parent;

  /** The Fts class Name */

  String className;	

  /** the variable name, if any */

  String variableName = null;

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


  /** x, y, width and height cache locally the geometrical properties, to speed
    up access; also, to prepare transition to beans model */

  protected int x = -1; 
  protected int y = -1 ;
  protected int width = -1;
  protected int height = -1;
  protected boolean isError = false;
  protected String  errorDescription;
  protected String font = null;
  protected int fontSize = -1;
  protected MaxData data;

  //
  //  Handling of properties
  //

  public final int getX()
  {
    return x;
  }

  public final void setX(int x)
  {
    if (this.x != x)
      {
	Fts.getServer().putObjectProperty(this, "x", x);
	this.x = x;
	setDirty();
      }
  }

  public final int getY()
  {
    return y;
  }

  public final void setY(int y)
  {
    if (this.y != y)
      {
	Fts.getServer().putObjectProperty(this, "y", y);
	this.y = y;
	setDirty();
      }
  }

  public final int getWidth()
  {
    return width;
  }

  public final void setWidth(int w)
  {
    if (this.width != w)
      {
	Fts.getServer().putObjectProperty(this, "w", w);
	this.width = w;
	setDirty();
      }
  }

  public final int getHeight()
  {
    return height;
  }

  public final void setHeight(int h)
  {
    if (this.height != h)
      {
	Fts.getServer().putObjectProperty(this, "h", h);
	this.height = h;
	setDirty();
      }
  }

  // Is Error is read only

  public final boolean isError()
  {
    return isError;
  }


  public final String getErrorDescription()
  {
    return errorDescription;
  }

  public final void updateErrorDescription()
  {
    Fts.getServer().askObjectProperty(this, "errdesc");
  }

  public final String getFont()
  {
    return font;
  }

  public final void setFont(String font)
  {
    if ((this.font == null) || (! this.font.equals(font)))
      {
	Fts.getServer().putObjectProperty(this, "font", font);
	this.font = font;
	setDirty();
      }
  }

  public final int getFontSize()
  {
    return fontSize;
  }

  public final void setFontSize(int fontSize)
  {
    if (this.fontSize != fontSize)
      {
	Fts.getServer().putObjectProperty(this, "fs", fontSize);
	this.fontSize = fontSize;
	setDirty();
      }
  }

  public MaxData getData()
  {
    return data;
  }

  public void updateData()
  {
    Fts.getServer().askObjectProperty(this, "data");
  }


  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object.
   */

  protected FtsObject(FtsObject parent, String className, String variableName, String description, int objId)
  {
    super();

    this.variableName = variableName;
    this.className    = className;
    this.description  = description;
    this.parent       = parent;

    if (objId != -1)
      setObjectId(objId);
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


  /* Accessors and selectors. */

  /** Get the object including patcher. */

  public final FtsObject getParent()
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

  /** Get the MaxDocument this objects is part of;
   * Temporary; actually the patcher document should be
   * a remote data and be known on the FTS side.
   */

  FtsPatcherDocument document;

  public MaxDocument getDocument()
  {
    if (document != null)
      return (MaxDocument) document;
    else if (parent != null)
      return parent.getDocument();
    else
      return null;
  }

  public void setDocument(MaxDocument document)
  {
    this.document = (FtsPatcherDocument) document;
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
      {
	document.setSaved(false);
      }
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

  /** Utility function Get the patcher data contaning the object if any */

  FtsPatcherData getPatcherData()
  {
    if ((parent != null) &&
	(parent.getData() != null) &&
	(parent.getData() instanceof FtsPatcherData))
      return (FtsPatcherData) parent.getData();
    else
      return null;
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
   * Dispose the data associated to the object, if any
   * can be called more than once.
   */

  void releaseData()
  {
    if ((data != null) && (data instanceof MaxData))
      Mda.dispose((MaxData) data);
  }

  /**
   * Delete the Java object, without touching the FTS object represented.
   * 
   */

  public void release()
  {
    // If we have data, dispose it, so that all
    // the editors will be closed.
    
    releaseData();

    parent.setDirty();

    // Take away the object from the container, if any

    if (getPatcherData() != null)
      getPatcherData().removeObject(this);

    // Fire also the global edit listeners

    Fts.fireObjectRemoved(this);

    // clean up to help the gc, and make the object
    // non functioning, so to catch use of the object
    // after the release/delete.

    parent = null;
    className = null;
    objectName = null;
    description = null;

    Fts.getServer().unregisterObject(this);
    setObjectId(-1);
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

  void handleMessage(FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
  }
}









