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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.editors.patcher.*;

/**
 * Proxy of an FTS patcher.
 */

public class FtsPatcherObject extends FtsObjectWithEditor implements MaxData
{  
  /** Patcher content: the window size and position */

  int windowX = 0;
  int windowY = 0;
  int windowHeight = 0;
  int windowWidth  = 0;

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
  
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsPatcherObject object
   */

  public FtsPatcherObject(Fts fts, FtsObject parent, String className, String variableName, String description)
  {
      super(fts, parent, variableName, className, description);
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

  /** Get the document this data belong to.
    Find it thru the patcher object */

  public final MaxDocument getDocument()
  {
    return super.getDocument();
  }

  /** get the patcher window x position */

  public final int getWindowX()
  {
    return windowX;
  }

  /** set the patcher window x position.
      Tell the server.
  */

  public final void setWindowX(int value)
  {
    // Not that changing the position do
    // not mark the file as dirty.
    if (windowX != value)
      {
	windowX = value;
	sendArgs[0].setInt(value);
	sendMessage(FtsObject.systemInlet, "set_wx", 1, sendArgs);
      }
  }

  /** get the patcher window y position */
  
  public final int getWindowY()
  {
    return windowY;
  }

  /** set the patcher window y position.
      Tell the server.
  */
  
  public final void setWindowY(int value)
  {
    // Not that changing the position do
    // not mark the file as dirty.
    if (windowY != value)
      {
	windowY = value;
	sendArgs[0].setInt(value);
	sendMessage(FtsObject.systemInlet, "set_wy", 1, sendArgs);
      }
  }

  /** get the patcher window height */

  public final int getWindowHeight()
  {
    return windowHeight;
  }
  

  /** set the patcher window height
   * Tell the server.
   */
  
  
  public final void setWindowHeight(int value)
  {
    if (windowHeight != value)
      {
	windowHeight = value;
	sendArgs[0].setInt(value);
	sendMessage(FtsObject.systemInlet, "set_wh", 1, sendArgs);
	
	setDirty();
      }
  }

  /** get the patcher window width */

  public final int getWindowWidth()
  {
    return windowWidth;
  }

  /** set the patcher window width
   * Tell the server.
   */

  public final void setWindowWidth(int value)
  {
    if (windowWidth != value)
      {
	windowWidth = value;
	sendArgs[0].setInt(value);
	sendMessage(FtsObject.systemInlet, "set_ww", 1, sendArgs);
	
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

  final void addObject(FtsObject obj)
  {
    objects.addElement(obj);
    fireObjectAdded(obj);
    getFts().fireObjectAdded(obj);
  }

  /** Add a connection to this patcher data */

  final void addConnection(FtsConnection c)
  {
    connections.addElement(c);
    fireConnectionAdded(c);
    getFts().fireConnectionAdded(c);
  }

  /** Remove an object to this patcher data */

  final void removeObject(FtsObject obj)
  {
    objects.removeElement(obj);
    fireObjectRemoved(obj);
  }

  /** Remove a connection to this patcher data */

  final void removeConnection(FtsConnection c)
  {
    connections.removeElement(c);
    fireConnectionRemoved(c);
  }

  /**
   * Redefine a patcher without looosing its content.
   */

  public void redefinePatcher(String description)
  {
    getFts().getServer().redefinePatcherObject(this, description);
    getFts().sync();

    setDirty();
  }

  public void requestShowObject(FtsObject obj)
  {
      sendArgs[0].setObject(obj);
      sendMessage(FtsObject.systemInlet, "show_object", 1, sendArgs);
  }
    
  FtsActionListener waitingListener;
  public void requestStopWaiting(FtsActionListener l)
  {
      waitingListener = l;
      sendMessage(FtsObject.systemInlet, "stop_waiting", 0, null);
  }

  public void setDescription(int nArgs, FtsAtom args[])
  {
    this.description = FtsParse.unparseArguments(nArgs, args);
  }

  /**  Ask fts to update the patcher; usefull after a paste */

  public void update()
  {
    sendMessage(FtsObject.systemInlet, "patcher_update", 0, null);
  }

  //
  // Updates Management
  //


  /** Tell FTS that this patcher is  "alive". 
   * Fts will send updates for this patcher.
   */

  public final void startUpdates()
  {
    sendMessage(FtsObject.systemInlet, "start_updates", 0, null);
  }


  /** Tell FTS that this patcher is not "alive".
   * Fts will stop sending updates for this patcher.
   */

  public final void stopUpdates()
  {
    sendMessage(FtsObject.systemInlet, "stop_updates", 0, null);
  }

 /**
  * Fts callback: open the editor associated with this FtsSequenceObject.
  * If not exist create them else show them.
  */
  public void openEditor(int nArgs, FtsAtom args[])
  {
      if(getEditorFrame() == null)	    
	  setEditorFrame(new ErmesSketchWindow(this));

      showEditor();
  }

  public void stopWaiting(int nArgs, FtsAtom args[])
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
  public void destroyEditor(int nArgs, FtsAtom args[])
  {
      disposeEditor();
  }

  public void showObject(int nArgs, FtsAtom args[])
  {
      if(getEditorFrame() != null)	    
	  ((ErmesSketchPad)((ErmesSketchWindow)getEditorFrame()).getEditor()).showObject((FtsObject)args[0].getObject());	      
  }
  public void redefineStart(int nArgs , FtsAtom args[])
  {
    objects.removeAllElements();
    connections.removeAllElements();
  }
  public void setRedefined(int nArgs , FtsAtom args[])
  {
    firePatcherChanged();
  }
  public void addObject(int nArgs , FtsAtom args[])
  {
    addObject(args[0].getObject());    
  }
  public void addConnection(int nArgs , FtsAtom args[])
  {
    addConnection(args[0].getConnection());    
  }
  public void setWX(int nArgs , FtsAtom args[])
  {
    windowX = args[0].getInt();      
  }
  public void setWY(int nArgs , FtsAtom args[])
  {
    windowY = args[0].getInt();      
  }
  public void setWW(int nArgs , FtsAtom args[])
  {
    windowWidth = args[0].getInt();      
  }
  public void setWH(int nArgs , FtsAtom args[])
  {
    windowHeight = args[0].getInt();      
  }
  public void setMessage(int nArgs , FtsAtom args[])
  {
    firePatcherHaveMessage(args[0].getString());
  }

  /////////////////////////////////////////
  public MaxData getData()
  {
    return this;
  }

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

  private final void fireObjectAdded(FtsObject object)
  {
    if (listener != null)
      listener.objectAdded(this, object);
  }

  private final void fireObjectRemoved(FtsObject object)
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


