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
import javax.swing.*;

import ircam.jmax.utils.*;
import ircam.jmax.mda.*;


/** Object set remote data class.
 *  
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
  
  /** Overwrite the super class fts.
    The object set need to be an edit listener, to autmatically
    delete a deleted object from the set.
    */

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

  /** get the content of the set as a list model */

  public ListModel getListModel()
  {
    return model;
  }

  /** execute remote calls */

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



