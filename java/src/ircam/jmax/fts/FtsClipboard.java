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
import ircam.jmax.utils.*;

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

  boolean empty = true; // Empty only before its first copy

  int copyCount = 0;

  public FtsClipboard(Fts fts, FtsObject parent, String variableName, String classname, int nArgs, FtsAtom args[])
  {
      super(fts, parent, null, classname, classname);
  }

  public void copy(FtsSelection sel)
  {
    empty = false;
    copyCount++;
    getFts().getServer().sendObjectMessage(this, -1, "copy", sel);
  }

  public void paste(FtsObject patcher)
  {
    getFts().getServer().sendObjectMessage(this, -1, "paste", patcher);
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






