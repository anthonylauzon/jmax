//
// jMax
// Copyright (C) 1999 by IRCAM
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

  protected  FtsClipboard(Fts fts, FtsObject parent, String className, String description, int objId)
  {
    super(fts, parent, className, null, description, objId);
  }

  public void copy(FtsSelection sel)
  {
    empty = false;
    copyCount++;
    fts.getServer().sendObjectMessage(this, -1, "copy", sel);
  }

  public void paste(FtsObject patcher)
  {
    fts.getServer().sendObjectMessage(this, -1, "paste", patcher);
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






