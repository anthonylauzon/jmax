package ircam.jmax.fts;

import java.io.*;
import java.util.*;
import com.sun.java.swing.*;

import ircam.jmax.utils.*;
import ircam.jmax.mda.*;


/** This class represent an object set in 
 *  FTS
 */

public class FtsObjectSet extends FtsRemoteData
{
  /** Key for remote calls */

  static final int REMOTE_CLEAN     = 1;
  static final int REMOTE_APPEND    = 2;
  static final int REMOTE_REMOVE_ONE    = 3;

  /** Keys for queries */

  static final int REMOTE_FIND          = 4;
  static final int REMOTE_FIND_ERRORS   = 5;
  static final int REMOTE_FIND_FRIENDS  = 6;

  MaxVector list;
  ObjectSetListModel model;
  FtsPatcherListener editListener;

  class ObjectSetListModel extends AbstractListModel
  {
    public java.lang.Object getElementAt(int index)
    {
      return list.elementAt(index);
    }

    public int getSize()
    {
      return list.size();
    }

    void listChanged()
    {
      fireContentsChanged(this, 0, list.size());
    }
  }

  class ObjectSetEditListener implements FtsPatcherListener
  {
    public void objectAdded(FtsPatcherData data, FtsObject object)
    {
    }

    public void objectRemoved(FtsPatcherData data, FtsObject object)
    {
      list.removeElement(object);
      model.listChanged();
    }

    public void connectionAdded(FtsPatcherData data, FtsConnection connection)
    {
    }

    public void connectionRemoved(FtsPatcherData data, FtsConnection connection)
    {
    }
    
    public void patcherChanged(FtsPatcherData data)
    {
    }
  }

  public FtsObjectSet()
  {
    super();

    list = new MaxVector();
    model = new ObjectSetListModel();
    editListener = new ObjectSetEditListener();
    Fts.addEditListener(editListener);
  }

  public void release()
  {
    Fts.removeEditListener(editListener);
    super.release();
  }
  /** Get the vector size */

  public ListModel getListModel()
  {
    return model;
  }

  /* a method inherited from FtsRemoteData */

  public final void call( int key, FtsMessage msg)
  {
    switch( key)
      {
      case REMOTE_CLEAN:
	list.removeAllElements();
	model.listChanged();
	break;

      case REMOTE_APPEND:
	Object value;

	value = msg.getNextArgument();

	while (value != null)
	  {
	    list.addElement(value);
	    value = msg.getNextArgument();
	  };

	model.listChanged();
	break;

      case REMOTE_REMOVE_ONE:
	list.removeElement(msg.getNextArgument());

      default:
	break;
      }
  }

  /* Client to server queries */

  public void find(FtsObject context, String name)
  {
    Object args[] = new Object[2];

    args[0] = context;
    args[1] = name;

    remoteCall(REMOTE_FIND, args);
    Fts.sync();
  }


  public void find(FtsObject context, Object values[])
  {
    remoteCall(REMOTE_FIND, values);
  }


  public void find(FtsObject context, MaxVector values)
  {
    remoteCall(REMOTE_FIND, context, values);
  }


  public void findErrors(FtsObject context)
  {
    remoteCall(REMOTE_FIND_ERRORS, context, (MaxVector) null);
  }


  public void findFriends(FtsObject target)
  {
    remoteCall(REMOTE_FIND_FRIENDS, target, (MaxVector) null);
  }
}



