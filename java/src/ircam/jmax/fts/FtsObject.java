//
// jmax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import java.io.*;
import java.lang.reflect.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.mda.*;
import javax.swing.*;
import javax.swing.filechooser.*; // tmp !!
import java.awt.*;
/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsObject 
{
  static final public int systemInlet = -1;
  
  private static Hashtable creators = new Hashtable();
  
  static private Class parameterTypes[] = new Class[2];
  static Object[] methodArgs = new Object[2];
  static Object[] methodArgs0 = new Object[2];
  static Object[] methodArgs1 = new Object[2];
  static Object[] methodArgs2 = new Object[2];
  static Object[] methodArgs3 = new Object[2];
  static Method methodCache = null;
  static String selectorCache = null;

  static {
    parameterTypes[0] = java.lang.Integer.TYPE;
    parameterTypes[1] = FtsAtom[].class;

    methodArgs0[0] = new Integer(0);
    methodArgs1[0] = new Integer(1);
    methodArgs2[0] = new Integer(2);    
    methodArgs3[0] = new Integer(3);    
  }

  static public void registerFtsObjectCreator(String nameclass, FtsObjectCreator creator)
  {
    creators.put(nameclass, creator);
  }

  /******************************************************************************/
  /*                                                                            */
  /*              STATIC FUNCTION                                               */
  /*                                                                            */
  /******************************************************************************/
  
  /** This function create an application layer object for an already existing
   * object in FTS; take directly the FtsStream as argument, where we are receiving
   * an object upload message.
   */

  /**
   * Send a "new object" messages to the server with arguments in form of an array of FtsAtom
   * used for objects created 
   *
   * @param patcher the parent patcher (can be null)
   * @param id the object id (object dosn't exist yet on server)
   * @param nArgs number of valid arguments in args array
   * @param args arguments of object creation
   */

  //static FtsObject makeFtsObjectFromMessage(Fts fts, FtsObject parent, FtsPatcherData data, 
  //int objId, String variable, String className, int nArgs, FtsAtom args[])

  static FtsObject makeFtsObjectFromMessage(Fts fts, FtsObject parent, FtsPatcherData data, 
					    int objId, String variableName, String className, int nArgs, FtsAtom args[])
    throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException, FtsException
  {
    Object creator;
    FtsObject obj;
    StringBuffer description;

    /* Note that we do the unparsing relative to ':' and variables
       here; in the future, a dedicated API should be used ! */
    
    // Here we do the mapping between fts names and application layer classes
    // Any addition of special classes should start by adding some lines
    // of code here.
    
    if (className != null)
      {
	Object ctr = creators.get(className);
	
	if(ctr != null)
	  obj = ((FtsObjectCreator)ctr).createInstance(fts, parent, variableName, className, nArgs, args);
	else if (className == "jpatcher")
	  obj =  new FtsPatcherObject(fts, parent, FtsParse.unparseArguments(nArgs, args));
	else if (className == "inlet")
	  obj =  new FtsInletObject(fts, parent, args[0].intValue);
	else if (className == "outlet")
	  obj =  new FtsOutletObject(fts, parent, args[0].intValue);
	else if (className == "fork")
	  obj =  new FtsForkObject(fts, parent, args[0].intValue);
	else if (className == "jcomment")
	  obj =  new FtsCommentObject(fts, parent);
	else if (className == "messbox")
	  obj =  new FtsMessageObject(fts, parent, FtsParse.unparseArguments(nArgs, args));
	else if (className == "slider")
	  obj =  new FtsSliderObject(fts, parent);
	else if (className == "intbox")
	  obj =  new FtsIntValueObject(fts, parent, className);
	else if (className == "toggle")
	  obj =  new FtsIntValueObject(fts, parent, className);
	else if (className == "button")
	  obj =  new FtsBangObject(fts, parent, className);
	else if (className == "floatbox")
	  obj =  new FtsFloatValueObject(fts, parent, className);
	else if (className == "__selection")
	  obj =  new FtsSelection(fts, parent);
	else if (className == "__clipboard")
	  obj =  new FtsClipboard(fts, parent);
	else
	  obj = new FtsObject(fts, parent, variableName, className, className + " " + FtsParse.unparseArguments(nArgs, args));
      }
    else
      obj = new FtsObject(fts, parent, null, null, "");
    
    obj.setObjectId(objId);
    
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
	  {
	    isError = false;
	  }
	else
	  {
	    isError = true;
	    parent.addErrorObject(this);
	  }

	if (listener instanceof FtsObjectErrorListener)
	  ((FtsObjectErrorListener)listener).errorChanged(isError);	  
      }
    else if (name == "fs")
      {
	fontSize = value;
      }
    else if (name == "fst")
      {
	fontStyle = value;
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
   * The object string description. Always set at object creation.
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
  protected int fontStyle = -1;
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

 /** Get the font style property */

  public final int getFontStyle()
  {
    return fontStyle;
  }

  /** Set the font size property. Tell it to the server, too. */

  public final void setFontStyle(int fontStyle)
  {
    if (this.fontStyle != fontStyle)
      {
	fts.getServer().putObjectProperty(this, "fst", fontStyle);
	this.fontStyle = fontStyle;
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
   * FtsObject creator (object without arguments)
   *
   * @param fts the server
   * @param parent the parent patcher
   * @param variableName the variable name or null
   * @param className the class name or null
   * @param description the object description
   */
    protected FtsObject(Fts fts, FtsObject parent, String variableName, String className, String description)
  {
    //super();

    this.fts = fts;
    this.parent = parent;

    /*if (objId != -1)
      setObjectId(objId);*/

    this.variableName = variableName;
    this.className = className;
    this.description = description;
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

  public final void setParent(FtsObject obj)
  {
    parent = obj;
  }

  /** Get the object class Name. */

  public final String getClassName()
  {
    if(className != null)
      return className;
    else
      return "";
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
    if(variableName == null)
      return description;
    else
      return variableName + " : " + description;
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

  public FtsPatcherData getPatcherData()
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
	if(hasErrorsInside())
	  parent.removeErrorObject(this);
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

    if(parent != null)
      {
	parent.setDirty();

	if((isError())||(hasErrorsInside()))
	  parent.removeErrorObject(this);
      }
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

  /**
   * send a message to an object on the server
   * used for objects created 
   *
   * @param inlet the inlet
   * @param selector the message selector
   * @param nArgs number of valid arguments in args array
   * @param args the message arguments
   */

  public final void sendMessage(int inlet, String selector, int nArgs, FtsAtom args[])
  {
    fts.getServer().sendObjectMessage(this, inlet, selector, nArgs, args);
  }

  /**
   * send a message without arguments to an object on the server
   * used for objects created 
   *
   * @param inlet the inlet
   * @param selector the message selector
   */

  public final void sendMessage(int inlet, String selector)
  {
    fts.getServer().sendObjectMessage(this, inlet, selector, 0, null);
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


  /** 
   * Handle a direct message from an FTS object.
   * Implementation based on Core Reflection API.
   *
   * @param  stream    a FtsStream on which to read the selector and the arguments
   * @exception java.io.IOException if an error occured during reading
   * @exception FtsQuittedException if the server has quitted
   * @exception java.io.InterruptedIOException if the read thread was interrupted
   */

  public void handleMessage(String selector, int nArgs, FtsAtom args[])
    throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    try
      {
	if(selector != selectorCache)
	  {
	    selectorCache = selector;
	    methodCache = getClass().getMethod(selector, parameterTypes);
	  }

	switch(nArgs)
	  {
	  case 0:
	    methodArgs0[1] = args;
	    methodCache.invoke(this, methodArgs0);
	    break;
	  case 1:
	    methodArgs1[1] = args;
	    methodCache.invoke(this, methodArgs1);
	    break;
	  case 2:
	    methodArgs2[1] = args;
	    methodCache.invoke(this, methodArgs2);
	    break;
	  case 3:
	    methodArgs3[1] = args;
	    methodCache.invoke(this, methodArgs3);
	    break;
	  default:
	    methodArgs[0] = new Integer(nArgs);
	    methodArgs[1] = args;
	    methodCache.invoke(this, methodArgs);
	  }	
      }
    catch ( IllegalAccessException exc)
      {
	System.err.println( exc);
      }
    catch ( IllegalArgumentException exc)
      {
	System.err.println( exc);
      }
    catch ( InvocationTargetException exc)
      {
	System.err.println( exc);
      }
    catch( NoSuchMethodException exc)
      {
	System.err.println( exc);
      }
    catch (SecurityException exc)
      {
	System.err.println( exc);
      }
  }

  /**
   * count of error-objects in a subpatcher or template (to paint this in orange !!!)
   */
  int errcount = 0;
  public void removeErrorObject(FtsObject obj)
  {
    if(!isARootPatcher())
      {
	if(errcount>0){
	  errcount--;
	  if (listener instanceof FtsObjectErrorListener)
	    {
	      if(errcount > 0)
		((FtsObjectErrorListener)listener).errorChanged(true);
	      else
		{
		  ((FtsObjectErrorListener)listener).errorChanged(false);
		  parent.removeErrorObject(this);
		}
	    }
		    
	}
      }
  }

  public void addErrorObject(FtsObject obj)
  {
    if(!isARootPatcher())
      {
	errcount++;
	if(errcount==1)
	  {
	    parent.addErrorObject(this);
		
	    if (listener instanceof FtsObjectErrorListener)
	      ((FtsObjectErrorListener)listener).errorChanged(true);
	  }
      }
  }

  public boolean hasErrorsInside()
  {
    return (errcount > 0);
  }
    
  public boolean isARootPatcher()
  {
    return (getParent() == getFts().getRootObject());
  }

    //////////////////////////////////////////////////////////////////////////////////////
    //// MESSAGES called from fts.
    //////////////////////////////////////////////////////////////////////////////////////

  /**
   * Fts callback: open the FileSave dialog and initialize it with default directory and fileName.
   */

  //final variables used by invokeLater method
  JFileChooser fd;
  Frame parentFrame;
  String dialogText;
  String callbackMethod;
  
  public void openFileDialog(int nArgs, FtsAtom args[])
  {
      parentFrame = null;
      callbackMethod = args[0].getString();
      dialogText = args[1].getString();
      String defaultPath = args[2].getString();
      String defaultName = args[3].getString();

      fd = new JFileChooser(defaultPath);
      fd.setDialogTitle(dialogText);
      fd.setSelectedFile(new File(defaultPath, defaultName));
      
      if(this instanceof FtsObjectWithEditor)
	  parentFrame = ((FtsObjectWithEditor)this).getEditorFrame();

      FtsObject current = this;
      while(((parentFrame==null)||(!parentFrame.isVisible()))&&(!isARootPatcher()))
      {
	  current = current.getParent();
	  if(current instanceof FtsObjectWithEditor)
	      parentFrame = ((FtsObjectWithEditor)current).getEditorFrame();
      }

      if(parentFrame!=null)
	  {
	      /*
		NOTE: we used invokeLater because the fileDialog is modal so when we show it via a
		message object (export ...), we lose a mouseup event on the message object leaving 
		the interactionEngine in an incorrect state (RunCtrlInteraction instead of RunModeInteraction)
		So invokeLater allow to consume the mouseUp event before fileDialog is shown
	      */
	      SwingUtilities.invokeLater(new Runnable() {
		    public void run()
			{ 
			   if (fd.showDialog(parentFrame, dialogText) == JFileChooser.APPROVE_OPTION)
			       {
				   String path = fd.getSelectedFile().getAbsolutePath();
				   sendArgs[0].setString(path); 
				   sendMessage(FtsObject.systemInlet, callbackMethod, 1, sendArgs);
			       } 
			}
		});
	  }
  }

  //----- Fields
  static FtsAtom[] sendArgs = new FtsAtom[128];
  static
  {
    for(int i=0; i<128; i++)
	sendArgs[i]= new FtsAtom();
  }
}









