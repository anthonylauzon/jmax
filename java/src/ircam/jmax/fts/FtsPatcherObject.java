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

package ircam.jmax.fts;

import java.io.*;
import java.util.*;
// import javax.swing.*;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import java.lang.reflect.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

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

  protected transient FtsArgs args = new FtsArgs();

  /*
    Patcher content: the edit mode;
    temporary hack, local, not propagated to fts,
    should disappear with the toolbar instead of the mode
    in the patcher editor.
  */
  
  final public static int UNKNOWN_MODE = 0;
  final public static int EDIT_MODE = 1;
  final public static int RUN_MODE  = 2;

  final public static int JMAX_FILE_TYPE  = 1;
  final public static int PAT_FILE_TYPE   = 0;
  
  //int editMode = UNKNOWN_MODE;
  int editMode = EDIT_MODE;

  /** Patcher content: objects */
  
  private MaxVector objects     = new MaxVector();
  
  /** Patcher content: connections */
  
  private MaxVector connections = new MaxVector();

  /** List of subPatchers */

  private MaxVector subPatchers = new MaxVector();

  /** true if patcher need to be saved */
  private boolean dirty = false;

  /** true if is already saved a first time */
  private boolean canSave = false;

  private String name = null;
  private int type = JMAX_FILE_TYPE;

  private boolean pasting = false;

  /******************************************************************************/
  /*                                                                            */
  /*              STATIC FUNCTION                                               */
  /*                                                                            */
  /******************************************************************************/
  
  /** This function create an application layer object for an already existing
   * object in FTS
   */

  static GraphicObject makeGraphicObjectFromServer(FtsServer server, FtsObject parent, int objId, String className, 
						   FtsAtom args[], int offset, int nArgs)
  {
    JMaxObjectCreator creator = null;

    if (className != null)
      {
	creator = JMaxClassMap.getCreator( className);

	if(creator != null)
	  return (GraphicObject)creator.create(server, parent, objId, className, args, offset, nArgs);	      
      }
    creator = JMaxClassMap.getCreator( "standard");
    if(creator != null)
      return (GraphicObject)creator.create(server, parent, objId, className, args, offset, nArgs);	      

    return null;
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
	  ((FtsPatcherObject)obj).addObject( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("removeObject"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  if(obj != JMaxApplication.getRootPatcher())
	    ((FtsPatcherObject)obj).releaseObject( ( FtsObject)args.getObject( 0));	  
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("addConnection"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).addConnection( args.getLength(), args.getAtoms());
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
	  //((FtsPatcherObject)obj).firePatcherChanged();
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("endUpload"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).endUpload();
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setDescription"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setDescription( args.getString( 0));
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
     FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setPatcherBounds"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setPatcherBounds( args.getInt( 0), args.getInt( 1), args.getInt( 2), args.getInt( 3));
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setMessage"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setMessage( args.getString( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setDirty"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setDirty( (args.getInt( 0) == 1)? true : false);
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("setSaved"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).setSaved( args.getInt( 0), args.getSymbol( 1).toString());
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("startPaste"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).startPaste();
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("endPaste"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPatcherObject)obj).endPaste();
	}
      });
    FtsObject.registerMessageHandler( FtsPatcherObject.class, FtsSymbol.get("noHelp"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  FtsGraphicObject object = ( FtsGraphicObject)args.getObject( 0);
	  JOptionPane.showMessageDialog(((ErmesSketchWindow)((FtsPatcherObject)obj).getEditorFrame()), 
					"Sorry, no help for object "+object.getClassName(), 
					"Warning", JOptionPane.INFORMATION_MESSAGE);
	}
      });
  }

  /*****************************************************************************************/
  /*                                 Global Edit Listening                                 */
  /*****************************************************************************************/

  static transient MaxVector editListeners = new MaxVector();

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

  public FtsPatcherObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom[] args, int offset, int length)
  {
    super(server, parent, id, className, (length > 0) ? args[offset].stringValue : null);
  }

  public FtsPatcherObject() throws IOException
  {
    super(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("jpatcher"));
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

  private void endUpload()
  {
    this.canSave = true;
    this.dirty = false;

    if( getEditorFrame() != null)
      {
	ErmesSketchPad sketch = ((ErmesSketchWindow)getEditorFrame()).itsSketchPad;
	sketch.getDisplayList().sortDisplayList();
	
	if(isARootPatcher())
	  sketch.setLocked(true);
	else
	  sketch.setLocked( ((FtsPatcherObject)getParent()).isLocked());
	
	ErmesSelection.patcherSelection.deselectAll();
      }
  }

  private void setDirty( boolean dirty)
  {
    this.dirty = dirty;
    if( getEditorFrame() != null)
      ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.setDirty(dirty);
  
    for( Enumeration e = subPatchers.elements(); e.hasMoreElements();)
      ((FtsPatcherObject) e.nextElement()).setDirty( dirty);
  }

  public boolean isDirty()
  {
    return dirty;
  }

  private void setSaved(int type, String name)
  {
    this.canSave = true;
    this.name = name;
    this.type = type;

    if(getEditorFrame()!=null)
      getEditorFrame().setTitle( name);

    if(isDirty())
      setDirty(false);    
  }

  public String getName()
  {
    return name;
  }

  public void setName( String name)
  {
    this.name = name;
  }

  public int getType()
  {
    return type;
  }

  public void setType( int type)
  {
    this.type = type;
  }

  public boolean canSave()
  {
    return canSave;
  }

  public void save()
  {
    save(this.type, this.name);
  }

  public void save( int type, String name)
  {
    args.clear();
    args.addInt( type);
    args.addSymbol( FtsSymbol.get( name));
    
    try{
      send( FtsSymbol.get("save"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending save Message!");
	e.printStackTrace(); 
      }
    this.name = name;
    this.type = type;
  }

  public final int getWindowX()
  {
    return windowX;
  }

  public final void setWindowX(int value)
  {
    if (windowX != value)
      {
	windowX = value;
	args.clear();
	args.addInt(windowX);
      
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
	args.addInt(windowY);
      
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
	args.addInt(windowHeight);
      
	try{
	  send( FtsSymbol.get("set_wh"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsPatcherObject: I/O Error sending set_wh Message!");
	    e.printStackTrace(); 
	  }
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
	args.addInt(windowWidth);
	
	try{
	  send( FtsSymbol.get("set_ww"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsPatcherObject: I/O Error sending set_ww Message!");
	    e.printStackTrace(); 
	  }
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

    if(obj instanceof FtsPatcherObject)
      {
	((FtsPatcherObject)obj).setDirty(isDirty());
	subPatchers.addElement(obj);
      }
    fireGlobalObjectAdded(obj);
  }

  /** Add a connection to this patcher data */

  final void addConnection(FtsConnection c)
  {
    connections.addElement(c);
    fireGlobalConnectionAdded(c);
  }

  /** Remove an object to this patcher data */

  public final void removeObject(FtsGraphicObject obj)
  {
    objects.removeElement(obj);

    if(obj instanceof FtsPatcherObject)
      subPatchers.removeElement(obj);

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
    args.clear();
    args.addRawString( description);
      
    try{
      send( FtsSymbol.get("set_arguments"), args);
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
    args.addObject(obj);
      
    try{
      send( FtsSymbol.get("show_object"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending show_object Message!");
	e.printStackTrace(); 
      }
  }
    
  transient FtsActionListener waitingListener;
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

  public void requestPaste( FtsClipboard clipboard, int dx, int dy)
  {
    args.clear();
    args.addObject( clipboard);
    args.addInt( dx);
    args.addInt( dy);

    try{
      send( FtsSymbol.get("paste"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending paste Message!");
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

  public final void requestSubPatcherUpload()
  {
    if(getEditorFrame() == null)
      {
	setEditorFrame( new ErmesSketchWindow(FtsPatcherObject.this));

	try{
	  send( FtsSymbol.get("upload"));
	}
	catch(IOException e)
	  {
	    System.err.println("FtsPatcherObject: I/O Error sending upload Message!");
	    e.printStackTrace(); 
	  }
      }
    requestOpenEditor();
  }

  public boolean isLocked()
  {
    if( getEditorFrame()!= null)
      return ((ErmesSketchWindow)getEditorFrame()).getSketchPad().isLocked();
    else
      return false;
  }

  public final void requestOpenHelpPatch( FtsObject obj)
  {
    args.clear();
    args.addObject( obj);
    try{
      send( FtsSymbol.get("open_help_patch"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending open_help_patch Message!");
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
  
  public void requestAddObject(String description, int x, int y, boolean doedit)
  {
    args.clear();
    args.addInt(x);
    args.addInt(y);
    args.addRawString( description);

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
    args.clear();
    args.addObject(oldObject);
    args.addRawString( description);

    try{
      send( FtsSymbol.get("redefine_object"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending redefine_object Message!");
	e.printStackTrace(); 
      }
  
    //removeObject(oldObject);
  }

  public void requestAddConnection(FtsGraphicObject from, int outlet, FtsGraphicObject to, int inlet)
  {
    args.clear();
    args.addObject(from);
    args.addInt(outlet);
    args.addObject(to);
    args.addInt(inlet);

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
    args.addObject(connection);
    try{
      send( FtsSymbol.get("delete_connection"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending delete_connection Message!");
	e.printStackTrace(); 
      }
  }

  public void requestDeleteObjects(Enumeration objects)
  {
    args.clear();

    for( ;objects.hasMoreElements();)      
      args.addObject( ((GraphicObject)objects.nextElement()).getFtsObject());      
      
    try{
      send( FtsSymbol.get("delete_objects"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending delete_objects Message!");
	e.printStackTrace(); 
      }
  }

 
  public void requestDeleteObject(FtsObject obj)
  {
    args.clear();
    args.addObject( obj);      
      
    try{
      send( FtsSymbol.get("delete_objects"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsPatcherObject: I/O Error sending delete_objects Message!");
	e.printStackTrace(); 
      }
  }

  //used in addObject method to start editing in added object if needed 
  public void addObject(int nArgs , FtsAtom args[]) 
  {
    int objId = args[0].intValue;
    int x = args[1].intValue;
    int y = args[2].intValue;
    int width = args[3].intValue;
    int height = args[4].intValue;
    int numIns = args[5].intValue;
    int numOuts = args[6].intValue;
    int layer = args[7].intValue;
    String errorDescription = args[8].symbolValue.toString();
    String className = args[9].symbolValue.toString();

    boolean isTemplate = (args[10].intValue == 1);

    GraphicObject newObj;
    
    if(isTemplate)
      newObj = new Standard( new FtsTemplateObject( getServer(), this, objId, className, args, 11, nArgs-11));
    else
      newObj = makeGraphicObjectFromServer( getServer(), this, objId, className, args, 11, nArgs-11);
    
    newObj.getFtsObject().setCurrentLayer( layer);

    newObj.setCurrentBounds( x, y, width, height);
    
    newObj.getFtsObject().setNumberOfInlets(numIns);
    newObj.getFtsObject().setNumberOfOutlets(numOuts);

    if( !errorDescription.equals( "no_error"))
      {
	newObj.getFtsObject().setError( 1);
	newObj.getFtsObject().setErrorDescription(errorDescription);
      }
    else
      newObj.getFtsObject().setError( 0);

    addObject(newObj.getFtsObject());

    boolean doEdit = false;
    if( ((FtsGraphicObject)newObj.getFtsObject()).getClassName() == null)
      doEdit = true;
    else
      {
	String descr = ((FtsGraphicObject)newObj.getFtsObject()).getDescription();
	if( descr!=null) descr.trim();
	if((descr!=null) && descr.equals(""))
	  doEdit = true;
      }
    newObj.getSketchPad().addNewObject(newObj, doEdit);

    if( pasting)
      ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.addPastedObject( newObj);
  }

  public void addConnection(int nArgs , FtsAtom args[])
  {
    if(nArgs==6)
      {
	FtsConnection connection = new FtsConnection( getServer(), this, args[0].intValue, 
						     (FtsGraphicObject)args[1].objectValue, args[2].intValue, 
						     (FtsGraphicObject)args[3].objectValue, args[4].intValue, 
						     args[5].intValue);   
	addConnection(connection);
	GraphicConnection gc = ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.addNewConnection(connection);
      
	if( pasting)
	  ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.addPastedConnection( gc);
      }
  }

  public void releaseConnection(FtsConnection c)
  {
    if( getEditorFrame() != null)
      ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.getDisplayList().remove( c);
  }

  public void objectRedefined(FtsGraphicObject obj)
  {
    ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.objectRedefined( obj);
  }

  public void releaseObject( FtsObject obj)
  {
    if(obj instanceof FtsConnection)
      {
	removeConnection((FtsConnection)obj);

	if( getEditorFrame() != null)
	  ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.getDisplayList().remove((FtsConnection)obj);
      }
    else
      {
	removeObject((FtsGraphicObject)obj);

	if( getEditorFrame() != null)
	  ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.getDisplayList().remove((FtsGraphicObject)obj);
      }
  }

  /**************************************************************/
  /**************************************************************/

  /**
  * Fts callback: open the editor associated with this FtsSequenceObject.
  * If not exist create them else show them.
  */
  public void openEditor(int nArgs, FtsAtom[] args)
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

  public void setDescription(String descr)
  {
    this.description = descr;
    if(getGraphicListener()!=null) getGraphicListener().redefined(this);
    if(getEditorFrame() != null) ((ErmesSketchWindow)getEditorFrame()).updateTitle(); 
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
  public void setPatcherBounds(int x, int y, int w, int h)
  {
    windowX = x;
    windowY = y;
    windowWidth = w;
    windowHeight = h;

    if(getEditorFrame() != null) ((ErmesSketchWindow)getEditorFrame()).setPatcherBounds( this); 
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

  // Paste

  void startPaste()
  {
    pasting = true;
  }

  void endPaste()
  {
    pasting = false;
    ((ErmesSketchWindow)getEditorFrame()).itsSketchPad.endPaste();
  }

  //
  // Edit And Change Listener
  //

  private transient FtsPatcherListener listener;

  // Set the patcher data listener 

  public void setPatcherListener(FtsPatcherListener listener)
  {
    this.listener = listener;
  }

  // Reset the patcher data listener 
  
  public void resetPatcherListener()
  {
    listener = null;
  }

  public void setNumberOfInlets(int ins)
  {
    super.setNumberOfInlets( ins);
    firePatcherChangedNumberOfInlets( ins);
  }

  final void firePatcherChangedNumberOfInlets(int ins)
  {
    if (listener != null)
      listener.patcherChangedNumberOfInlets(this, ins);
  }

  public void setNumberOfOutlets(int outs)
  {
    super.setNumberOfOutlets( outs);
    firePatcherChangedNumberOfOutlets( outs);
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


