package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/** This class represent the content of a patcher.
 *  Geometry ???
 *
 * The patcher data content is handled completely in FTS;
 * objects are always created on the fts side and then uploaded,
 * and then added to the container by FTS.
 * To be seen for object creation at the editor level, if we call
 * anyway the patcher listener.
 * At the distruction, objects are taken away from the container
 * by FTS, either all together, or one at a time.
 * 
 * Also for the Paste, objects and connections created in an exported
 * patcher are automatically uploaded.
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

  /** Patcher content: the container object  */

  FtsObject container;

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

  public final FtsObject getContainerObject()
  {
    return container; 
  }

  public final MaxVector getObjects()
  {
    return objects;
  }

  public final MaxVector getConnections()
  {
    return connections;
  }

  public final MaxDocument getDocument()
  {
    return container.getDocument();
  }

  
  final void addObject(FtsObject obj)
  {
    objects.addElement(obj);
    fireObjectAdded(obj);
  }

  final void addConnection(FtsConnection c)
  {
    connections.addElement(c);
    fireConnectionAdded(c);
  }

  final void removeObject(FtsObject obj)
  {
    objects.removeElement(obj);
    fireObjectRemoved(obj);
  }

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


  /** Close tell FTS that this patcher is  "alive". */

  public final void startUpdates()
  {
    remoteCall(REMOTE_START_UPDATES);
  }


  /** Close tell FTS that this patcher is not "alive". */

  public final void stopUpdates()
  {
    remoteCall(REMOTE_STOP_UPDATES);
  }

  //
  // Edit And Change Listener
  //

  FtsPatcherListener listener;

  public void setPatcherListener(FtsPatcherListener listener)
  {
    this.listener = listener;
  }

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

    // Fire also the global edit listeners

    Fts.fireObjectRemoved(this, object);
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

  private final void firePatcherChanged()
  {
    if (listener != null)
      listener.patcherChanged(this);
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

  public final void call( int key, FtsMessage msg)
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
	container = (FtsObject) msg.getNextArgument();
	break;

      case REMOTE_ADD_OBJECT:
	addObject((FtsObject) msg.getNextArgument());
	break;

      case REMOTE_ADD_CONNECTION:
	addConnection((FtsConnection) msg.getNextArgument());
	break;

      default:
	break;
      }
  }
}


