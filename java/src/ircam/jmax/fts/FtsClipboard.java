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

import ircam.fts.client.*;
import ircam.jmax.*;

/**
 * Implement an interface to the FTS clipboard.
 * Support two methods: copy from a FtsSelection, and paste to an FtsContainer 
 */

public class FtsClipboard  extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a Fts clipboard;
   */

  protected FtsArgs args = new FtsArgs();
  protected boolean empty = true; // Empty only before its first copy
  protected int copyCount = 0;

  public FtsClipboard() throws IOException
  {
      super(JMaxApplication.getServer(), JMaxApplication.getServer().getRoot(), FtsSymbol.get("__clipboard"));
  }

  public void copy(FtsSelection sel)
  {
    args.clear();
    args.add(sel);
      
    try{
      send( FtsSymbol.get("copy"), args);
    }
    catch(IOException e)
      {
	System.err.println("[FtsClipboard]: I/O Error sending copy message!");
	e.printStackTrace(); 
      }

    empty = false;
    copyCount++;
  }

  public void paste(FtsObject patcher)
  {
    args.clear();
    args.add(patcher);

    try{
      send( FtsSymbol.get("paste"), args);
    }
    catch(IOException e)
      {
	System.err.println("[FtsClipboard]: I/O Error sending paste message!");
	e.printStackTrace(); 
      }
  }

  public int getCopyCount()
  {
    return copyCount;
  }

  public boolean isEmpty()
  {
    return empty;
  }
}






