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

import java.util.*;
import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.utils.*;

/** Object set class.
 *  
 */

public class FtsObjectSet extends FtsObject implements ListModel
{
  MaxVector list, dataListeners;
  FtsEditListener editListener;

  class ObjectSetEditListener implements FtsEditListener
  {
    public void objectAdded(FtsObject object)
    {
    }

    public void objectRemoved(FtsObject object)
    {
      list.removeElement(object);
      fireListChanged();
    }

    public void connectionAdded(FtsConnection connection)
    {
    }

    public void connectionRemoved(FtsConnection connection)
    {
    }
  }

  public FtsObjectSet(Fts fts, FtsObject parent, String variableName, String classname, int nArgs, FtsAtom args[])
  {
      super(fts, parent, variableName, classname, "");

      list = new MaxVector();
      dataListeners = new MaxVector();
      
      editListener = new ObjectSetEditListener();
      getFts().addEditListener(editListener);
  }
  
  public void release()
  {
      getFts().removeEditListener(editListener);
      super.release();
  }

  /** listmodel interface */
  public Object getElementAt(int index)
  {
      return list.elementAt(index);
  }
  public int getSize()
  {
      return list.size();
  }
  public void addListDataListener(ListDataListener l)
  {
      dataListeners.addElement(l);
  }
  public void removeListDataListener(ListDataListener l)
  {
      dataListeners.removeElement(l);
  }
  private void fireListChanged()
  {
      ListDataEvent evt = new ListDataEvent(this, ListDataEvent.CONTENTS_CHANGED, 0, getSize());
      for(Enumeration e = dataListeners.elements(); e.hasMoreElements();)
	  ((ListDataListener)e.nextElement()).contentsChanged(evt);
  }

  /* SERVER CALLBACK */
  public void clear(int nArgs , FtsAtom args[])
  {
    list.removeAllElements();
    fireListChanged();
  }
  public void append(int nArgs , FtsAtom args[])
  {
      for(int i=0; i<nArgs; i++)	  
	  list.addElement((FtsObject)args[i].getObject());

      fireListChanged();
  }
  public void remove(int nArgs , FtsAtom args[])
  {
      list.removeElement((FtsObject)args[0].getObject());
  }

  /* Client to server queries */

  public void find(FtsObject context, String name)
  {
      sendArgs[0].setObject(context);
      sendArgs[1].setString(name);
      sendMessage(FtsObject.systemInlet, "objectset_find", 2, sendArgs);
  }

  public void find(FtsObject context, Object values[])
  {
      sendArgs[0].setObject(context);
      int i=0;
      for(i=0; i<values.length && i<sendArgs.length-1; i++)
	  sendArgs[i+1].setString((String)values[i]);
      
      sendMessage(FtsObject.systemInlet, "objectset_find", i+1, sendArgs);
  }


  public void find(FtsObject context, MaxVector values)
  {
      sendArgs[0].setObject(context);
      int i=0;
   
      for(i=0; i<values.size() && i<sendArgs.length-1; i++)
	  sendArgs[i+1].setString((String)values.elementAt(i));
      
      sendMessage(FtsObject.systemInlet, "objectset_find", i+1, sendArgs);
  }


  public void findErrors(FtsObject context)
  {
      sendArgs[0].setObject(context);
      sendMessage(FtsObject.systemInlet, "objectset_find_errors", 1, sendArgs);
  }


  public void findFriends(FtsObject target)
  {
      sendArgs[0].setObject(target);
      sendMessage(FtsObject.systemInlet, "objectset_find_friends", 1, sendArgs);
  }
}



