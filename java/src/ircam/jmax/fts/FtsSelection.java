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

import ircam.jmax.*;
import ircam.fts.client.*;
/**
 * Implement the selection.
 * Only one instance for server is built.
 */

public class FtsSelection extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /** The objects (and connections) selected in the patcher */

  private MaxVector objects     = new MaxVector();

  protected transient FtsArgs args = new FtsArgs();

  /**
   * Create a Fts selection;
   */
  
  public FtsSelection() throws IOException
  {
    super(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("__selection")); 
  }

  /** Add an object to this container  */

  final public void add(FtsObject obj)
  {
    objects.addElement(obj);

    args.clear();
    args.add(obj);
      
    try{
	send( FtsSymbol.get("add"), args);
    }
    catch(IOException e)
    {
	System.err.println("[FtsSelection]: I/O Error sending add message!");
	e.printStackTrace(); 
    } 
  }

  /** Remove an object from this container. */

  final public void remove(FtsObject obj)
  {
    if (objects.contains(obj))
      {
	objects.removeElement(obj);
	
	args.clear();
	args.add(obj);
	
	try{
	    send( FtsSymbol.get("remove"), args);
	}
	catch(IOException e)
	    {
		System.err.println("[FtsSelection]: I/O Error sending remove message!");
		e.printStackTrace(); 
	    }      
      }
  }

  /** Get the objects */

  final public MaxVector getObjects()
  {
    return objects;
  }

  /** clean: remove everything */

  final public void clean()
  {
    objects.removeAllElements();

    try{
      send( FtsSymbol.get("clear"));
    }
    catch(IOException e)
      {
	System.err.println("[FtsSelection]: I/O Error sending clean message!");
	e.printStackTrace(); 
      }
  }
}






