//
// jMax
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
// Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
// 

package ircam.jmax.fts;

import java.io.*;
import java.util.*;
import javax.swing.*;
import java.lang.reflect.*;

import ircam.ftsclient.*;
import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.GraphicObject;

/**
 * Proxy of an FTS patcher.
 */

public class FtsPatcherObject extends FtsObjectWithEditor 
{  
  /** Patcher content: the window size and position */

  int windowX = 0;
  int windowY = 0;
  int windowHeight = 0;
  int windowWidth  = 0;

  protected FtsArgs args = new FtsArgs();

  /*
    Patcher content: the edit mode;
    temporary hack, local, not propagated to fts,
    should disappear with the toolbar instead of the mode
    in the patcher editor.
  */
  
  final public static int UNKNOWN_MODE = 0;
  final public static int EDIT_MODE = 1;
  final public static int RUN_MODE  = 2;
  
  int editMode = UNKNOWN_MODE;

  /** Patcher content: objects */
  
  private MaxVector objects     = new MaxVector();
  
  /** Patcher content: connections */
  
  private MaxVector connections = new MaxVector();

  /******************************************************************************/
  /*                                                                            */
  /*              STATIC FUNCTION                                               */
  /*                                                                            */
  /******************************************************************************/
  
  /** This function create an application layer object for an already existing
   * object in FTS
   */

  static FtsGraphicObject makeFtsObjectFromServer(FtsServer server, FtsObject parent, int objId, String className, 
						  int nArgs, FtsAtom args[])
  {
    FtsGraphicObject obj = null;

    if (className != null)
      {
	Class theClass = ObjectCreatorManager.getFtsClass(className);
	if(theClass != null)
	  {
	    Object[] arg = new Object[] {server, parent, FtsSymbol.get(className), new Integer(nArgs), 
					 args, new Integer(objId)};
	    Class[] cls = new Class[] { ircam.ftsclient.FtsServer.class, ircam.ftsclient.FtsObject.class, 
					ircam.ftsclient.FtsSymbol.class, java.lang.Integer.TYPE, 
					ircam.ftsclient.FtsAtom[].class, java.lang.Integer.TYPE};
	    try{
	      Constructor constr = theClass.getConstructor(cls);

	      if(constr != null)
		obj = (FtsGraphicObject)(constr.newInstance(arg));

	    } catch (NoSuchMethodException e) {
	      System.out.println(e);
	    } catch (InstantiationException e) {
	      System.out.println(e);
	    } catch (IllegalAccessException e) {
	      System.out.println(e);
	    } catch (InvocationTargetException e) {
	      System.out.println(e);
	      e.printStackTrace();
	    } 
	  }
	else if (className == "jpatcher")
	  obj =  new FtsPatcherObject(server, parent, FtsSymbol.get(className), nArgs, args, objId);
	else if (className == "inlet")
	  obj =  new FtsInletObject(server, parent, FtsSymbol.get(className), nArgs, args, objId);
	else if (className == "outlet")
	  obj =  new FtsOutletObject(server, parent, FtsSymbol.get(className), nArgs, args, objId);
	else
	  obj = new FtsGraphicObject(server, parent, FtsSymbol.get(className), nArgs, args, objId);
      }
    else
      obj = new FtsGraphicObject(server, parent, FtsSymbol.get(className), nArgs, args, objId);

    return obj;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               MessageHandlers                             */
  /*                                                                           */
  /*****************************************************************************/

  static
  {
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("addObject"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  System.err.println("[FtsPatcherObject] : invoke addObject ");
	  ((FtsPatcherObject)obj).addObject( args);
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("redefineObject"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).redefineObject( args);
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("redefineTemplateObject"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).redefineTemplateObject( args);
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("addConnection"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).addConnection( args);
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("objectRedefined"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).objectRedefined( (FtsGraphicObject)args.getObject(0));
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("stopWaiting"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).stopWaiting();
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("showObject"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).showObject( (FtsGraphicObject)args.getObject( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("redefineStart"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).redefineStart();
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setRedefined"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).firePatcherChanged();
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setDescription"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setDescription( args);
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setWX"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setWX( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setWY"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setWY( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setWW"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setWW( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setWH"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setWH( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setMessage"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setMessage( args.getString( 0));
	}
      });
  }

  /*****************************************************************************************/
  /*                                 Global Edit Listening                                 */
  /*****************************************************************************************/

  static MaxVector editListeners = new MaxVector();

  static public void addGlobalEditListener(FtsEditListener listener)
  {
    editListeners.addElement(listener);
  }
    
  static public void removeGlobalEditListener(FtsEditListener listener)
  {
    editListeners.removeElement(listener);
  }
    
  static public void fireGlobalObjectRemoved(FtsGraphicObject object)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).objectRemoved(object);
  }
  static public void fireGlobalObjectAdded(FtsGraphicObject object)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).objectAdded(object);
  }
  static public void fireGlobalConnectionAdded(FtsConnection connection)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).connectionAdded(connection);
  }
  static public void fireGlobalConnectionRemoved(FtsConnection connection)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).connectionRemoved(connection);
  }
  static public void fireGlobalAtomicAction(boolean active)
  {
    for (int i = 0; i < editListeners.size(); i++)
      ((FtsEditListener) editListeners.elementAt(i)).atomicAction(active);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsPatcherObject object
   */

  public FtsPatcherObject(FtsServer server, FtsObject parent, FtsSymbol className, FtsArgs args) throws IOException
  {
    super(server, parent, className, args);
  }
  public FtsPatcherObject(FtsServer server, FtsObject parent, FtsSymbol className) throws IOException
  {
    super(server, parent, className);
  }
  public FtsPatcherObject(FtsServer server, FtsObject parent, FtsSymbol className, int nArgs, FtsAtom[] args, int id)
  {
    super(server, parent, className, nArgs, args, id);
  }
  public FtsPatcherObject() throws IOException
  {
    super(MaxApplication.getServer(), MaxApplication.getServer().getRoot(), FtsSymbol.get("jpatcher"));
  }
  /** Get all the objects in this patcherdata */

  public final MaxVector getObjects()
  {
    return objects;
  }

  /** Get all the connections in this patcherdata */

  public final MaxVector getConnections()
  {
    return connections;
  }

  public final int getWindowX()
  {
    return windowX;
  }

  public final void setWindowX(int value)
  {
    // Not that changing the position do
    // not mark the file as dirty.
    if (windowX != value)
      {
	windowX = value;
	args.clear();
	args.add(windowX);
      
	try{
	  send( FtsSymbol.get("set_wx"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsPatcherObject: I/O Error sending set_wx Message!");
	    e.printStackTrace(); 
	  }
      }
  }
  public final int getWindowY()
  {
    return windowY;
  }
  public final void setWindowY(int value)
  {
    if (windowY != value)
      {
	windowY = value;
	args.clear();
	args.add(windowY);
      
	try{
	  send( FtsSymbol.get("set_wy"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsPatcherObject: I/O Error sending set_wy Message!");
	    e.printStackTrace(); 
	  }
      }
  }
  public final int getWindowHeight()
  {
    return windowHeight;
  }
  public final void setWindowHeight(int value)
  {
    if (windowHeight != value)
      {
	windowHeight = value;

	args.clear();
	args.add(windowHeight);
      
	try{
	  send( FtsSymbol.get("set_wh"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsPatcherObject: I/O Error sending set_wh Message!");
	    e.printStackTrace(); 
	  }
	
	setDirty();
      }
  }
  public final int getWindowWidth()
  {
    return windowWidth;
  }
  public final void setWindowWidth(int value)
  {
    if (windowWidth != value)
      {
	windowWidth = value;

	args.clear();
	args.add(windowWidth);
      
	try{
	  send( FtsSymbol.get("set_ww"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsPatcherObject: I/O Error sending set_ww Message!");
	    e.printStackTrace(); 
	  }
	
	setDirty();
      }
  }

  /** Get the edit mode of this patcher data.
   * The edit mode is stored in the patcher data 
   * because we need to know it in order to decide
   * the initial mode of a new patcher editor.
   */

  public final int getEditMode()
  {
    return editMode;
  }

  /** Get the edit mode of this patcher data,
   * or, if unknown, the first known edit mode in
   * its patcher ancestor chain.
   * The edit mode is stored in the patcher data 
   * because we need to know it in order to decide
   * the initial mode of a new patcher editor.
   */

  public final int getRecursiveEditMode()
  {
    if (editMode == UNKNOWN_MODE)
      {
	if (getParent() != null)
	  if (getParent() instanceof FtsPatcherObject)
	    return ((FtsPatcherObject) getParent()).getRecursiveEditMode();

	return UNKNOWN_MODE;
      }
    else
      return editMode;
  }

  /** Set the edit mode of this patcher data.
   * The edit mode is stored in the patcher data 
   * because we need to know it in order to decide
   * the initial mode of a new patcher editor.
   */

  public final void setEditMode(int value)
  {
    editMode = value;
  }


  /** Add an object to this patcher data */

  final void addObject(FtsGraphicObject obj)
  {
    objects.addElement(obj);
    fireObjectAdded(obj, doedit);
    fireGlobalObjectAdded(obj);
    doedit = false;
  }

  /** Add a connection to this patcher data */

  final void addConnection(FtsConnection c)
  {
    connections.addElement(c);
    fireConnectionAdded(c);
    fireGlobalConnectionAdded(c);
  }

  /** Remove an object to this patcher data */

  final void removeObject(FtsGraphicObject obj)
  {
    objects.removeElement(obj);
    fireObjectRemoved(obj);
    fireGlobalObjectRemoved(obj);
  }

  /** Remove a connection to this patcher data */

  final void removeConnection(FtsConnection c)
  {
    connections.removeElement(c);
    fireGlobalConnectionRemoved(c);
  }

  /**
   * Redefine a patcher without looosing its content.
   */

  public void redefinePatcher(String description)
  {
    MaxVector vec = new MaxVector();
    FtsParse.parseAtoms(description, vec);

    args.clear();
    for(int i=0; i<vec.size(); i++)
      args.add(vec.elementAt(i));
      
    try{
      send( FtsSymbol.get("redefine_patcher"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending redefine_patcher Message!");
	e.printStackTrace(); 
      }
  }

  public void requestShowObject(FtsGraphicObject obj)
  {
    args.clear();
    args.add(obj);
      
    try{
      send( FtsSymbol.get("show_object"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending show_object Message!");
	e.printStackTrace(); 
      }
  }
    
  FtsActionListener waitingListener;
  public void requestStopWaiting(FtsActionListener l)
  {
    waitingListener = l;
    try{
      send( FtsSymbol.get("stop_waiting"));
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending stop_waiting Message!");
	e.printStackTrace(); 
      }
  }

  /**  Ask fts to update the patcher; usefull after a paste */

  public void update()
  {
    try{
      send( FtsSymbol.get("patcher_update"));
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending patcher_update Message!");
	e.printStackTrace(); 
      }
  }

  //
  // Updates Management
  //

  /** Tell FTS that this patcher is  "alive". 
   * Fts will send updates for this patcher.
   */

  public final void startUpdates()
  {
    try{
      send( FtsSymbol.get("start_updates"));
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending start_updates Message!");
	e.printStackTrace(); 
      }
  }


  /** Tell FTS that this patcher is not "alive".
   * Fts will stop sending updates for this patcher.
   */

  public final void stopUpdates()
  {
    try{
      send( FtsSymbol.get("stop_updates"));
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending stop_updates Message!");
	e.printStackTrace(); 
      }
  }
  
  /***************************************************************/
  /*************** ASYNCRONOUS ADDING/REMOVING *******************/
  
  public void requestAddObject(String description, int x, int y, boolean doedit)
  {
    MaxVector vec = new MaxVector();
    FtsParse.parseAtoms(description, vec);

    args.clear();
    args.add(x);
    args.add(y);
    //args.add(doEdit);//????????????????????

    for(int i=0; i<vec.size(); i++)
      args.add(vec.elementAt(i));
      
    try{
      send( FtsSymbol.get("add_object"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending add_object Message!");
	e.printStackTrace(); 
      }
  }

  public void requestRedefineObject(FtsGraphicObject oldObject, String description)
  {
    MaxVector vec = new MaxVector();
    FtsParse.parseAtoms(description, vec);
     
    args.clear();
    args.add(oldObject);
    args.add(getServer().getNewObjectID());

    for(int i=0; i<vec.size(); i++)
      args.add(vec.elementAt(i));
      
    try{
      send( FtsSymbol.get("redefine_object"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending redefine_object Message!");
	e.printStackTrace(); 
      }
  
    removeObject(oldObject);
  }

  public void requestAddConnection(FtsGraphicObject from, int outlet, FtsGraphicObject to, int inlet)
  {
    args.clear();
    args.add(getServer().getNewObjectID());
    args.add(from);
    args.add(outlet);
    args.add(to);
    args.add(inlet);

    try{
      send( FtsSymbol.get("add_connection"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending add_connection Message!");
	e.printStackTrace(); 
      }
  }

  public void requestDeleteConnection(FtsConnection connection)
  {
    args.clear();
    args.add(connection);
    try{
      send( FtsSymbol.get("delete_connection"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending delete_connection Message!");
	e.printStackTrace(); 
      }
  }

  //used in addObject method to start editing in added object if needed 
  boolean doedit = false;
  public void addObject(int nArgs , FtsAtom args[]) 
  {
    System.err.println("[FtsPatcherObject]: addObject nArgs "+nArgs);

    if((nArgs==1)&&(args[0].isObject()))
      addObject((FtsGraphicObject)args[0].objectValue);
    else
      {
	int objId = args[0].intValue;
	int x = args[1].intValue;
	int y = args[2].intValue;
	      
	System.err.println("[FtsPatcherObject]: addObject id "+objId+" x "+x+" y "+y);

	String className = null;
	//doedit = false;
	int startIndex, numArgs = 0;
	FtsAtom[] arguments = null;
	if(nArgs>3)
	  {
	    //if(args[3].isString()) 
	    if(args[3].isSymbol()) 
	      {
		//className = args[3].stringValue;
		className = args[3].symbolValue.toString();
		numArgs = nArgs-4;
		startIndex = 4; 
	      }
	    else
	      { 
		numArgs = nArgs-3;
		startIndex = 3;
	      }
		      
	    arguments = new FtsAtom[numArgs];
	    for(int i=0; i<numArgs; i++)
	      arguments[i] = args[startIndex+i];
	  }
	      
	System.err.println("[FtsPatcherObject]: addObject className "+className+" numArgs "+numArgs);

	FtsGraphicObject newObj = makeFtsObjectFromServer(getServer(), this, objId, className, numArgs, arguments);
	      
	newObj.setX(x);
	newObj.setY(y);
	      
	addObject(newObj);
      }
  }

  public void redefineObject(int nArgs , FtsAtom args[]) 
  {
    String className = null;
    String variable = null;
    int startIndex, numArgs;
    //boolean defVar = (args[0].intValue==1)? true : false;
    int newObjId = args[0].intValue;
      
    if(args[1].isString()) 
      {
	className = args[1].stringValue;
	numArgs = nArgs-2;
	startIndex = 2;
      }
    else
      { 
	numArgs = nArgs-1;
	startIndex = 1;
      }
      
    FtsAtom[] arguments = new FtsAtom[numArgs];
    for(int i=0; i<numArgs; i++)
      arguments[i] = args[startIndex+i];
	
    FtsGraphicObject newObj = makeFtsObjectFromServer(getServer(), this, newObjId, className, numArgs, arguments);
      
    addObject(newObj);
  }

  public void redefineTemplateObject(int nArgs , FtsAtom args[]) 
  {
    String className = null;
    String variable = null;
    int startIndex, numArgs;
    //boolean defVar = (args[0].getInt()==1)? true : false;
    int newObjId = args[0].intValue;
    
    if(args[1].isString()) 
      {
	className = args[1].stringValue;
	numArgs = nArgs-2;
	startIndex = 2;
      }
    else
      { 
	numArgs = nArgs-1;
	startIndex = 1;
      }
	
    FtsAtom[] arguments = new FtsAtom[numArgs];
    for(int i=0; i<numArgs; i++)
      arguments[i] = args[startIndex+i];
	
    FtsGraphicObject newObj = new FtsTemplateObject(getServer(), this, FtsSymbol.get(className), numArgs, arguments, newObjId);
      
    addObject(newObj);
  }

  public void addConnection(int nArgs , FtsAtom args[])
  {
    if(nArgs==1)
      addConnection((FtsConnection)args[0].objectValue);
    else
      if(nArgs==6)
	addConnection(new FtsConnection(getServer(), this, args[0].intValue, 
					(FtsGraphicObject)args[1].objectValue, args[2].intValue, 
					(FtsGraphicObject)args[3].objectValue, args[4].intValue, args[5].intValue));
    setDirty();
  }

  public void objectRedefined(FtsGraphicObject obj)
  {
    ((FtsGraphicObject)obj).setDefaults();
    fireObjectRedefined(obj);
  }

  /**************************************************************/
  /**************************************************************/

  /**
  * Fts callback: open the editor associated with this FtsSequenceObject.
  * If not exist create them else show them.
  */
  public void openEditor(int argc, FtsAtom[] argv)
  {
    if(getEditorFrame() == null) 
      {
	SwingUtilities.invokeLater(new Runnable() {
	    public void run() {
	      setEditorFrame(new ErmesSketchWindow(FtsPatcherObject.this));
	      showEditor();
	    }
	  });
      }
    else
      showEditor();
  }

  public void stopWaiting()
  {
    if(waitingListener != null)
      { 
	waitingListener.ftsActionDone();
	waitingListener = null;
      }      
    else
      if(getEditorFrame()!=null)
	((ErmesSketchWindow)getEditorFrame()).itsSketchPad.stopWaiting();
  }
  /**
   * Fts callback: destroy the editor associated with this FtsSequenceObject.
   */
  public void destroyEditor()
  {
    disposeEditor();
  }

  public void showObject(FtsGraphicObject obj)
  {
    if(getEditorFrame() != null)	    
      ((ErmesSketchPad)((ErmesSketchWindow)getEditorFrame()).getEditor()).showObject(obj);	      
  }
  public void redefineStart()
  {
    objects.removeAllElements();
    connections.removeAllElements();
  }

  public void setDescription(int nArgs, FtsAtom args[])
  {
    this.description = FtsParse.unparseArguments(nArgs, args);
    if(getGraphicListener()!=null) getGraphicListener().redefined(this);
    if(getEditorFrame() != null) ((ErmesSketchWindow)getEditorFrame()).updateTitle(); 

    setDirty();
  }
  public void setWX(int value)
  {
    windowX = value;      
  }
  public void setWY(int value)
  {
    windowY = value;      
  }
  public void setWW(int value)
  {
    windowWidth = value;      
  }
  public void setWH(int value)
  {
    windowHeight = value;      
  }
  public void setMessage(String message)
  {
    firePatcherHaveMessage(message);
  }

  /////////////////////////////////////////
  public void updateData()
  {
    update();
  }

  //
  // Edit And Change Listener
  //

  private FtsPatcherListener listener;

  /** Set the patcher data listener */

  public void setPatcherListener(FtsPatcherListener listener)
  {
    this.listener = listener;
  }

  /** Reset the patcher data listener */

  public void resetPatcherListener()
  {
    listener = null;
  }

  private final void fireObjectAdded(FtsGraphicObject object, boolean doedit)
  {
    if (listener != null)
      listener.objectAdded(this, object, doedit);
  }

  private final void fireObjectRedefined(FtsGraphicObject newObject)
  {
    if (listener != null)
      listener.objectRedefined(this, newObject);
  }

  private final void fireObjectRemoved(FtsGraphicObject object)
  {
    if (listener != null)
      listener.objectRemoved(this, object);

  }

  private final void fireConnectionAdded(FtsConnection connection)
  {
    if (listener != null)
      listener.connectionAdded(this, connection);
  }

  private final void fireConnectionRemoved(FtsConnection connection)
  {
    if (listener != null)
      listener.connectionRemoved(this, connection);
  }

  final void firePatcherChangedNumberOfInlets(int ins)
  {
    if (listener != null)
      listener.patcherChangedNumberOfInlets(this, ins);
  }

  final void firePatcherChangedNumberOfOutlets(int outs)
  {
    if (listener != null)
      listener.patcherChangedNumberOfOutlets(this, outs);
  }

  private final void firePatcherChanged()
  {
    if (listener != null)
      listener.patcherChanged(this);
  }
	
  private final void firePatcherHaveMessage(String msg)
  {
    if (listener != null)
      listener.patcherHaveMessage(msg);
  }
}


