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
// Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
// 

package ircam.jmax.fts;

import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;
import ircam.fts.client.*;
import ircam.jmax.*;

/** Object set class.
 *  
 */

public class FtsRuntimeErrors extends FtsObject implements ListModel
{
  Vector list, dataListeners;

  static
  {
    FtsObject.registerMessageHandler( FtsRuntimeErrors.class, FtsSymbol.get("postError"), new FtsMessageHandler() {
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsRuntimeErrors)obj).postError( (FtsGraphicObject)args.getObject( 0), args.getString( 1));
	}
      });
  }

  public FtsRuntimeErrors() throws IOException
  {
    super(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("__runtimeerrors"));
      
    list = new Vector();
    dataListeners = new Vector();
  
    FtsPatcherObject.addGlobalEditListener(new FtsEditListener()
      {
	public void objectAdded(FtsObject object){}
	public void objectRemoved(FtsObject object)
	{
	  removeErrorsOf(object);
	  fireListChanged();
	}
	public void connectionAdded(FtsConnection connection){}
	public void connectionRemoved(FtsConnection connection){}
	public void atomicAction(boolean active){}    
      });
  }
  
  public void postError(FtsGraphicObject obj, String description)
  {
    RuntimeError err = new RuntimeError(obj, description);
      
    RuntimeError oldErr = getSamePostedError(err);
    if(oldErr!=null)
      {
	list.removeElement(oldErr);
	err.setCounter(oldErr.getCount());
      }
      
    err.incrementCounter();
    list.addElement(err);
    fireListChanged();
  }
  
  public void removeErrors(int[] ids)
  {
    if(ids.length>0)
      {
	for(int i = ids.length-1; i>=0; i--)
	  list.removeElementAt(ids[i]);
	      
	fireListChanged();
      }
  }
 
  public boolean errorPosted(RuntimeError err)
  {
    for(Enumeration e = list.elements(); e.hasMoreElements();)
      if(((RuntimeError)e.nextElement()).equals(err))
	return true;
    return false;
  }
  public void removeErrorsOf(FtsObject obj)
  {
    RuntimeError err;
    for(int i=list.size()-1; i >= 0; i--)
      {
	err = (RuntimeError)list.elementAt(i);
	if(err.getObject()==obj)
	  list.removeElementAt(i);
      }	  
  }

  public RuntimeError getSamePostedError(RuntimeError err)
  {
    RuntimeError runerr;
    for(Enumeration e = list.elements(); e.hasMoreElements();)
      {
	runerr = (RuntimeError)e.nextElement();
	if(runerr.equals(err))
	  return runerr;
      }
    return null;
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
