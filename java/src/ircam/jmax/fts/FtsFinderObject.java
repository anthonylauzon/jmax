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

import ircam.jmax.*;

/** Object set class.
 *  
 */

public class FtsFinderObject extends FtsObject
{
  public FtsFinderObject(Fts fts, FtsObject parent, String variableName, String classname, int nArgs, FtsAtom args[])
  {
      super(fts, parent, variableName, classname, "");
  }
  
  /* Client to server queries */

  public void find(FtsObject context, FtsObjectSet set, String name)
  {
      sendArgs[0].setObject(set);
      sendArgs[1].setObject(context);
      sendArgs[2].setString(name);
      sendMessage(FtsObject.systemInlet, "finder_find", 3, sendArgs);
  }

  public void find(FtsObject context, FtsObjectSet set, Object values[])
  {
      sendArgs[0].setObject(set);
      sendArgs[1].setObject(context);

      int i=0;
      for(i=0; i<values.length && i<sendArgs.length-1; i++)
	  sendArgs[i+2].setString((String)values[i]);
      
      sendMessage(FtsObject.systemInlet, "finder_find", i+2, sendArgs);
  }


  public void find(FtsObject context, FtsObjectSet set, MaxVector values)
  {
      sendArgs[0].setObject(set);
      sendArgs[1].setObject(context);

      int i=0;
   
      for(i=0; i<values.size() && i<sendArgs.length-1; i++)
	  sendArgs[i+2].setString((String)values.elementAt(i));

      sendMessage(FtsObject.systemInlet, "finder_find", i+2, sendArgs);
  }

  public void findFriends(FtsObject target, FtsObjectSet set)
  {
    sendArgs[0].setObject(set);
    sendArgs[1].setObject(target);
    sendMessage(FtsObject.systemInlet, "finder_find_friends", 2, sendArgs);
  }
}






