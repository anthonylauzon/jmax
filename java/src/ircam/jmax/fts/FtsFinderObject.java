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

import java.util.*;
import java.io.*;

import ircam.fts.client.*;
import ircam.jmax.*;

/** Object set class.
 *  
 */

public class FtsFinderObject extends FtsObject
{
  public FtsFinderObject() throws IOException
  {
    super(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("__finder"));
  }
  
  public void find(FtsObjectSet set, String query)
  {
    find( JMaxApplication.getRootPatcher(), set, query);
  }

  public void find(FtsObject context, FtsObjectSet set, String query)
  {
    args.clear();
    args.add(set);
    args.add(context);
    args.addRawString( query);
      
    try{
      send( FtsSymbol.get("finder_find"), args);
    }
    catch(IOException e)
      {
	System.err.println("[FtsFinderObject]: I/O Error sending find message!");
	e.printStackTrace(); 
      }
  }

  public void findFriends(FtsObject target, FtsObjectSet set)
  {
    args.clear();
    args.add(set);
    args.add(target);

    try{
      send( FtsSymbol.get("finder_find_friends"), args);
    }
    catch(IOException e)
      {
	System.err.println("[FtsFinderObject]: I/O Error sending find_friends message!");
	e.printStackTrace(); 
      }
  }

  protected FtsArgs args = new FtsArgs();
}






