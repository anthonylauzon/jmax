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

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/** 
 * The Java Implementation for the atom list remote data class.
 */

public class FtsAtomList extends FtsRemoteData
{
  /* Keys for remote calls */

  static final int REMOTE_UPDATE = 1;
  static final int REMOTE_SET    = 2;
  static final int REMOTE_NAME   = 3;
  
  String name; // the list name (read only)
  MaxVector values = new MaxVector();

  public FtsAtomList()
  {
    super();
  }

  /** Return the size of the atom list */

  public int getSize()
  {
    return values.size();
  }

  /** Get the content of the atom list as a MaxVector */

  public MaxVector getValues()
  {
    return values;
  }

  /* Access the name */
  public String getName()
  {
    return name;
  }

  /** Get the content of the atom list as text. */

  public String getValuesAsText()
  {
    return FtsParse.unparseDescription(values);
  }


  /** Set the content of the atom list as text. */
  
  public void setValuesAsText(String value)
  {
    values.removeAllElements();
    FtsParse.parseAtoms(value, values);
    changed();
  }


  /** Update the content of the atom list from the server. */

  public void forceUpdate()
  {
    remoteCall(REMOTE_UPDATE);
    getFts().sync();
  }


  /** 
   * Send the content of the atom list to the server.
   */

  public void changed()
  {
    remoteCall(REMOTE_SET, values);
  }


  /** Implement the remote calls from the server */

  protected final void call( int key, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch( key)
      {
      case REMOTE_SET:

	values.removeAllElements();

	while (! stream.endOfArguments())
	  values.addElement(stream.getNextArgument());

	break;

      case REMOTE_NAME:
	name = stream.getNextStringArgument();
	break;    	

      default:
	break;
      }
  }
}


