//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 
package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/**
 * Remote data class representing the content of a patcher.
 *
 * The patcher data content is handled completely in FTS;
 * objects are always created on the fts side and then uploaded,
 * and then added to the container by FTS.
 * At the distruction, objects are taken away from the container
 * by FTS, either all together, or one at a time.
 * 
 * Also for the Paste, objects and connections created in an exported
 * patcher are automatically uploaded.
 *
 * Also the geometrical property of a patcher window are handled at this
 * level; a patcher editor is essentially an editor of an instance of FtsPatcherData.
 */

public class FtsPatcherData extends FtsRemoteData
{
  /** Key for remote calls */

  static final int REMOTE_REDEFINE_START = 0;
  static final int REMOTE_REDEFINED      = 1;
  static final int REMOTE_SET_CONTAINER  = 2;
  static final int REMOTE_ADD_OBJECT     = 3;
  static final int REMOTE_ADD_CONNECTION = 4;
  static final int REMOTE_START_UPDATES  = 5;
  static final int REMOTE_STOP_UPDATES   = 6;
  static final int REMOTE_UPDATE         = 7;

  static final int REMOTE_SET_WX         = 8;
  static final int REMOTE_SET_WY         = 9;
  static final int REMOTE_SET_WW         = 10;
  static final int REMOTE_SET_WH         = 11;

  static final int REMOTE_MESSAGE        = 12;

  /** Patcher content: the container object  */

  FtsObject container;

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

  public FtsPatcherData()
  {
    super();
  }

  //
  //  Content Management and Acess
  // 

  /** get the patcher this patcherdata corresponds to */

  public final FtsObject getContainerObject()
  {
    return container; 
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
    return container.getDocument();
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
	remoteCall(REMOTE_SET_WX, value);
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
	remoteCall(REMOTE_SET_WY, value);
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
	remoteCall(REMOTE_SET_WH, value);
	container.setDirty();
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
	remoteCall(REMOTE_SET_WW, value);
	container.setDirty();
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
	if (container.getParent() != null)
	  if (container.getParent().getData() != null)
	    if (container.getParent().getData() instanceof FtsPatcherData)
	      return ((FtsPatcherData) container.getParent().getData()).getRecursiveEditMode();

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
  }

  /** Add a connection to this patcher data */

  final void addConnection(FtsConnection c)
  {
    connections.addElement(c);
    fireConnectionAdded(c);
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


  /**  Ask fts to update the patcher; usefull after a paste */

  public void update()
  {
    remoteCall(REMOTE_UPDATE);
  }

  //
  // Updates Management
  //


  /** Tell FTS that this patcher is  "alive". 
   * Fts will send updates for this patcher.
   */

  public final void startUpdates()
  {
    remoteCall(REMOTE_START_UPDATES);
  }


  /** Tell FTS that this patcher is not "alive".
   * Fts will stop sending updates for this patcher.
   */

  public final void stopUpdates()
  {
    remoteCall(REMOTE_STOP_UPDATES);
  }

  //
  // Edit And Change Listener
  //

  FtsPatcherListener listener;

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

  //
  // Remote Data management
  // 

  public void release()
  {
    objects = null;
    connections = null;
    super.release();
  }

  /** Execute remote calls */

  public final void call( int key, FtsStream msg)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch( key)
      {
      case REMOTE_REDEFINE_START:
	objects.removeAllElements();
	connections.removeAllElements();
	break;

      case  REMOTE_REDEFINED:
	firePatcherChanged();
	break;

      case REMOTE_SET_CONTAINER:
	container = msg.getNextObjectArgument();
	break;

      case REMOTE_ADD_OBJECT:
	addObject(msg.getNextObjectArgument());
	break;

      case REMOTE_ADD_CONNECTION:
	addConnection(msg.getNextConnectionArgument());
	break;

      case REMOTE_SET_WX:
	windowX = msg.getNextIntArgument();
	break;
      case REMOTE_SET_WY:
	windowY = msg.getNextIntArgument();
	break;
      case REMOTE_SET_WW:
	windowWidth = msg.getNextIntArgument();
	break;
      case REMOTE_SET_WH:
	windowHeight = msg.getNextIntArgument();
	break;
      case REMOTE_MESSAGE:
	firePatcherHaveMessage(msg.getNextStringArgument());
	break;
      default:
	break;
      }
  }
}





