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

package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.fts.client.*;
import ircam.jmax.*;

public class FtsUpdateGroup extends FtsObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsUpdateGroup.class, FtsSymbol.get("update_group_begin"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  for( Iterator i =  ((FtsUpdateGroup)obj).updateGroupList.iterator(); i.hasNext(); )
	    ((FtsUpdateGroupListener)i.next()).updateGroupStart();
	}
      });
    FtsObject.registerMessageHandler( FtsUpdateGroup.class, FtsSymbol.get("update_group_end"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  for( Iterator i =  ((FtsUpdateGroup)obj).updateGroupList.iterator(); i.hasNext(); )
	    ((FtsUpdateGroupListener)i.next()).updateGroupEnd();
	}
      });
  }
    
  public FtsUpdateGroup() throws IOException
  {
    super(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("update_group"));
  }
  
  public void start()
  {
    try
      {
	send( FtsSymbol.get("start"));
      }
    catch(IOException e)
      {
	System.err.println("[FtsUpdateGroup]: I/O Error sending Start Message!");
	e.printStackTrace(); 
      }
  }

  /* Sketchpad List */
  
  public void add( FtsUpdateGroupListener listener)
  {
    updateGroupList.add( listener);
  }

  public void remove( FtsUpdateGroupListener listener)
  {
    updateGroupList.remove( listener);
  }

  ArrayList updateGroupList = new ArrayList();
}



