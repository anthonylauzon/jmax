package ircam.jmax.fts;

import java.io.*;
import java.util.*;
import com.sun.java.swing.*;

import ircam.jmax.utils.*;
import ircam.jmax.mda.*;


/** This class represent an Integer vector in 
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

  public FtsObjectSet()
  {
    super();

    list = new MaxVector();
    model = new ObjectSetListModel();
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
	break;

      case REMOTE_APPEND:
	int size;

	size = msg.getNumberOfArguments();

	for (int i = 0 ; i < size - 2; i++)
	  list.addElement(msg.getNextArgument());
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
    model.listChanged();
  }


  public void find(FtsObject context, Object values[])
  {
    remoteCall(REMOTE_FIND, values);
    Fts.sync();
    model.listChanged();
  }


  public void find(FtsObject context, MaxVector values)
  {
    remoteCall(REMOTE_FIND, context, values);
    Fts.sync();
    model.listChanged();
  }


  public void findErrors(FtsObject context)
  {
    remoteCall(REMOTE_FIND_ERRORS, context, (MaxVector) null);
    Fts.sync();
    model.listChanged();
  }


  public void findFriends(FtsObject target)
  {
    remoteCall(REMOTE_FIND_FRIENDS, target, (MaxVector) null);
    Fts.sync();
    model.listChanged();
  }
}



