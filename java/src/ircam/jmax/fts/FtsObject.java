//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
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

  /** This function create an application layer object for an already existing
   * object in FTS; take directly the FtsStream as argument, where we are receiving
   * an object upload message.
   */

  static FtsObject makeFtsObjectFromMessage(Fts fts, FtsStream stream, boolean doVariable)
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
      return new FtsObject(fts, parent, "", null, "", objId);

    /* Get the class name */

    if (! stream.nextIsSymbol())
      {
	obj = new FtsObject(fts, parent, "", variable,
			    FtsParse.unparseObjectDescription(stream), objId);
      }
    else
      {
	className = stream.getNextSymbolArgument();
	
	/* Note that we do the unparsing relative to ':' and variables
	   here; in the future, a dedicated API should be used ! */

	// Here we do the mapping between fts names and application layer classes
	// Any addition of special classes should start by adding some lines
	// of code here.

	if (className == "jpatcher")
	  {
	    if (doVariable)
	      obj =  new FtsPatcherObject(fts, parent, variable,
					  variable + " : " + FtsParse.unparseObjectDescription(stream), objId);
	    else
	      obj =  new FtsPatcherObject(fts, parent, variable,
					  FtsParse.unparseObjectDescription(stream), objId);
	  }
	else if (className == "inlet")
	  obj =  new FtsInletObject(fts, parent, stream.getNextIntArgument(), objId);
	else if (className == "outlet")
	  obj =  new FtsOutletObject(fts, parent, stream.getNextIntArgument(), objId);
	else if (className == "messbox")
	  obj =  new FtsMessageObject(fts, parent, FtsParse.unparseObjectDescription(stream), objId);
	else if (className == "jcomment")
	  obj =  new FtsCommentObject(fts, parent, FtsParse.simpleUnparseObjectDescription(stream), objId);
	else if (className == "intbox")
	  obj =  new FtsIntValueObject(fts, parent, className, "intbox", objId);
	else if (className == "toggle")
	  obj =  new FtsIntValueObject(fts, parent, className, "toggle", objId);
	else if (className == "button")
	  obj =  new FtsIntValueObject(fts, parent, className, "button", objId);
	else if (className == "slider")
	  obj =  new FtsSliderObject(fts, parent, "slider", objId);
	else if (className == "floatbox")
	  obj =  new FtsFloatValueObject(fts, parent, className, "floatbox", objId);
	else if (className == "__selection")
	  obj =  new FtsSelection(fts, parent, className, "__selection", objId);
	else if (className == "__clipboard")
	  obj =  new FtsClipboard(fts, parent, className, "__clipboard", objId);
	else
	  {
	    if (doVariable)
	      obj = new FtsObject(fts, parent, className, variable,
				  variable + " : " + FtsParse.unparseObjectDescription(className, stream), objId);
	    else
	      obj = new FtsObject(fts, parent, className, variable,
				  FtsParse.unparseObjectDescription(className, stream), objId);
	  }
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


  /** Set an integer property coming from the server.
   *  Called by FtsServer when a property value message is received
   * from the server; handle the basic property by name, that are mapped
   * to beans like property for FtsObject instances.
   * Subclasses can specialize the method (calling super.localPut !!)
   * in order to add special property handling.
   * 
   * Responsable for firing the properties listener.
   * In order to simplify property listener handling, and reduce
   * memory overhead, we support only one listener for each instance
   * of FtsObject; this listener can listen for many different kind of event;
   * the listener is fired on a given event if it implements the correct interface
   * for handling the event.
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
    else if (name == "layer")
      {
	layer = value;
      }
  }


  /** Set an integer property coming from the server.
   * Currently, no property have a float type.
   */

  protected void localPut(String name, float value)
  {
    // No float property at this level
  }


  /** Set a non integer nor float property coming from the server.
   * 
   * See the other version of localPut.
   */

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
    else if (name == "data")
      {
	data = (MaxData) value;
	fts.fireNewDataListenerOn(this, data);
      }
    else if (name == "comment")
      {
	comment = (String) value;
      }
  }


  protected Object listener;

  /** Set the unique object listener */

  public void setObjectListener(Object obj)
  {
    listener = obj;
  }

  /** Get the current object listener */

  public Object getObjectListener()
  {
    return listener;
  }

  /******************************************************************************/
  /*                                                                            */
  /*              Object Variables                                              */
  /*                                                                            */
  /******************************************************************************/

  /** The Fts Server this object belong to */

  Fts fts;

  /** Fts Object ID  */

  private int ftsId = -1;

  /** A flag to handle double deletes from the editor; should be done better */

  private boolean deleted = false;

  /** The parent object. Usually the container patcher or abstraction or template */

  FtsObject parent;

  /** The Fts class Name */

  String className;	

  /** the variable name, if any */

  String variableName = null;

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
  protected int layer = -1;
  protected MaxData data;
  protected String comment;

  //
  //  Handling of properties
  //

  /** Get the X property */

  public final int getX()
  {
    return x;
  }


  /** Set the X property. Tell it to the server, too. */

  public final void setX(int x)
  {
    if (this.x != x)
      {
	fts.getServer().putObjectProperty(this, "x", x);
	this.x = x;
	setDirty();
      }
  }

  /** Get the Y property */

  public final int getY()
  {
    return y;
  }

  /** Set the Y property. Tell it to the server, too. */

  public final void setY(int y)
  {
    if (this.y != y)
      {
	fts.getServer().putObjectProperty(this, "y", y);
	this.y = y;
	setDirty();
      }
  }

  /** Get the Width property */

  public final int getWidth()
  {
    return width;
  }


  /** Set the Width property. Tell it to the server, too. */

  public final void setWidth(int w)
  {
    if (this.width != w)
      {
	fts.getServer().putObjectProperty(this, "w", w);
	this.width = w;
	setDirty();
      }
  }

  /** Get the Height property */

  public final int getHeight()
  {
    return height;
  }


  /** Set the Height property. Tell it to the server, too. */

  public final void setHeight(int h)
  {
    if (this.height != h)
      {
	fts.getServer().putObjectProperty(this, "h", h);
	this.height = h;
	setDirty();
      }
  }


  /** Get the Error property. Error is a read only property. */

  public final boolean isError()
  {
    return isError;
  }

  /** Get the error description property. Error description is a read only property. */

  public final String getErrorDescription()
  {
    return errorDescription;
  }

  /** Get the font property */

  public final String getFont()
  {
    return font;
  }

  /** Set the font property. Tell it to the server, too. */

  public final void setFont(String font)
  {
    if ((this.font == null) || (! this.font.equals(font)))
      {
	fts.getServer().putObjectProperty(this, "font", font);
	this.font = font;
	setDirty();
      }
  }

  /** Get the font size property */

  public final int getFontSize()
  {
    return fontSize;
  }

  /** Set the font size property. Tell it to the server, too. */

  public final void setFontSize(int fontSize)
  {
    if (this.fontSize != fontSize)
      {
	fts.getServer().putObjectProperty(this, "fs", fontSize);
	this.fontSize = fontSize;
	setDirty();
      }
  }

  /** Get the layer  property */

  public final int getLayer()
  {
    return layer;
  }

  /** Set the layer property. Tell it to the server, too. */

  public final void setLayer(int layer)
  {
    if (this.layer != layer)
      {
	fts.getServer().putObjectProperty(this, "layer", layer);
	this.layer = layer;
	setDirty();
      }
  }


  /** Set the color property. Tell it to the server.
    Colors are not locally stored, can only be set, and they are meaningfull only
    for some object
    */

  public final void setColor(int color)
  {
    fts.getServer().putObjectProperty(this, "color", color);
    setDirty();
  }

  /** Get the data property of this object.*/

  public MaxData getData()
  {
    return data;
  }

  /** Ask the server to update the data property of this object.*/

  public void updateData()
  {
    fts.getServer().askObjectProperty(this, "data");
  }

  /** Get the comment property of this object.*/

  public String getComment()
  {
    return comment;
  }

  /** Set the comment property. Tell it to the server, too. */

  public void setComment(String v)
  {
    comment = v;
    fts.getServer().putObjectProperty(this, "comment", comment);
  }

  /** Get the FTS instance this object belong to */

  public Fts getFts()
  {
    return fts;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object.
   */

  protected FtsObject(Fts fts, FtsObject parent, String className, String variableName, String description, int objId)
  {
    super();

    this.fts       = fts;
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

  /** Get the patcher including this object. */

  public final FtsObject getParent()
  {
    return parent;
  }

  /** Get the object class Name. */

  public final String getClassName()
  {
    return className;
  }

  FtsPatcherDocument document;

  /** Get the MaxDocument this objects is part of;
   * Actually the patcher document should be
   * a remote data and be known on the FTS side.
   */

  public MaxDocument getDocument()
  {
    if (document != null)
      return (MaxDocument) document;
    else if (parent != null)
      return parent.getDocument();
    else
      return null;
  }

  /** Set the Max document this obect belong to */

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
    if (! deleted)
      {
	fts.getSelection().removeObject(this);
	parent.setDirty();
	fts.getServer().deleteObject(this);
      }
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

    deleted = true;

    releaseData();

    parent.setDirty();

    // Take away the object from the container, if any

    if (getPatcherData() != null)
      getPatcherData().removeObject(this);

    // Fire also the global edit listeners

    fts.fireObjectRemoved(this);

    // clean up to help the gc, and make the object
    // non functioning, so to catch use of the object
    // after the release/delete.

    parent = null;
    className = null;
    description = null;

    fts.getServer().unregisterObject(this);
    setObjectId(-1);
  }

  // Communication with the object

  /** Send a message to an object in the server. */

  public final void sendMessage(int inlet, String selector, MaxVector args)
  {
    fts.getServer().sendObjectMessage(this, inlet, selector, args);
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









