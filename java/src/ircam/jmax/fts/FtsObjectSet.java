package ircam.jmax.fts;

import java.io.*;
import java.util.*;
import com.sun.java.swing.*;

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

  DefaultListModel list = new DefaultListModel();

  public FtsObjectSet()
  {
    super();
  }

  /** SHould go away !!! */

  public String getName()
  {
    return "objectset";
  }

  /** Get the vector size */

  public ListModel getListModel()
  {
    return list;
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

	for (int i = 0 ; i < size; i++)
	  list.addElement(msg.getArgument(i + 2));
	break;

      case REMOTE_REMOVE_ONE:
	list.removeElement(msg.getArgument(2));

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


  public void find(FtsObject context, String names[])
  {
    Object args[] = new Object[names.length + 1];

    args[0] = context;

    for (int i = 0; i < names.length; i++)
      args[i + 1] = names[i];

    remoteCall(REMOTE_FIND, args);
    Fts.sync();
  }


  public void findErrors(FtsObject context)
  {
    Object args[] = new Object[1];

    args[0] = context;

    remoteCall(REMOTE_FIND_ERRORS, args);
    Fts.sync();
  }


  public void findFriends(FtsObject context, FtsObject target)
  {
    Object args[] = new Object[2];

    args[0] = context;
    args[1] = target;

    remoteCall(REMOTE_FIND_FRIENDS, args);
    Fts.sync();
  }
}



