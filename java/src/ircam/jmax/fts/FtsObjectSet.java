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
import javax.swing.*;

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
  FtsEditListener editListener;

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

  class ObjectSetEditListener implements FtsEditListener
  {
    public void objectAdded(FtsObject object)
    {
    }

    public void objectRemoved(FtsObject object)
    {
      list.removeElement(object);
      model.listChanged();
    }

    public void connectionAdded(FtsConnection connection)
    {
    }

    public void connectionRemoved(FtsConnection connection)
    {
    }
  }

  public FtsObjectSet()
  {
    super();

    list = new MaxVector();
    model = new ObjectSetListModel();
  }
  
  public void setFts(Fts fts)
  {
    super.setFts(fts);

    editListener = new ObjectSetEditListener();
    fts.addEditListener(editListener);
  }

  public void release()
  {
    fts.removeEditListener(editListener);
    super.release();
  }
  /** Get the vector size */

  public ListModel getListModel()
  {
    return model;
  }

  /* a method inherited from FtsRemoteData */

  public final void call( int key, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch( key)
      {
      case REMOTE_CLEAN:
	list.removeAllElements();
	model.listChanged();
	break;

      case REMOTE_APPEND:

	while (! stream.endOfArguments())
	  list.addElement(stream.getNextObjectArgument());

	model.listChanged();
	break;

      case REMOTE_REMOVE_ONE:
	list.removeElement(stream.getNextObjectArgument());

      default:
	break;
      }
  }

  /* Client to server queries */

  public void find(FtsObject context, String name)
  {
    remoteCallStart(REMOTE_FIND);
    remoteCallAddArg(context);
    remoteCallAddArg(name);
    remoteCallEnd();

    fts.sync();
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
    remoteCallStart(REMOTE_FIND_ERRORS);
    remoteCallAddArg(context);
    remoteCallEnd();
  }


  public void findFriends(FtsObject target)
  {
    remoteCallStart(REMOTE_FIND_FRIENDS);
    remoteCallAddArg(target);
    remoteCallEnd();
  }
}



