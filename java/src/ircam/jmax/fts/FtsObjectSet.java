//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.fts;

import java.util.*;
import java.io.*;

// import javax.swing.*;
import javax.swing.ListModel;

// import javax.swing.event.*;
import javax.swing.event.ListDataListener;
import javax.swing.event.ListDataEvent;

import ircam.fts.client.*;
import ircam.jmax.*;

/** Object set class.
 *  
 */

public class FtsObjectSet extends FtsObject implements ListModel
{
  MaxVector list, dataListeners;
  FtsEditListener editListener;

  class ObjectSetEditListener implements FtsEditListener
  {
    public void objectAdded(FtsObject object){}
    public void objectRemoved(FtsObject object)
    {
      list.removeElement(object);
      fireListChanged();
    }
    public void connectionAdded(FtsConnection connection){}
    public void connectionRemoved(FtsConnection connection){}
    public void atomicAction(boolean active){}      
  }

  static
  {
    FtsObject.registerMessageHandler( FtsObjectSet.class, FtsSymbol.get("clear"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsObjectSet)obj).clear();
	}
      });
    FtsObject.registerMessageHandler( FtsObjectSet.class, FtsSymbol.get("append"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsObjectSet)obj).append( args);
	}
      });
    FtsObject.registerMessageHandler( FtsObjectSet.class, FtsSymbol.get("remove"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsObjectSet)obj).removeObject( args.getObject( 0));
	}
      });
  }


  public FtsObjectSet() throws IOException
  {
    super(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("__objectset"));

    list = new MaxVector();
    dataListeners = new MaxVector();
      
    editListener = new ObjectSetEditListener();
    FtsPatcherObject.addGlobalEditListener(editListener);
  }
  
  public void release()
  {
    FtsPatcherObject.removeGlobalEditListener(editListener);
  }

  public void append(FtsArgs args)
  {
    for(int i=0; i<args.getLength(); i++)
      list.addElement(args.getObject(i));

    fireListChanged();
  }

  public void clear()
  {
    list.removeAllElements();
    fireListChanged();
  }

  public void removeObject(FtsObject obj)
  {
    list.removeElement(obj);
    fireListChanged();
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
}

