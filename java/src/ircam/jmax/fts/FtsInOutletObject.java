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

/**
 * SuperClass for proxies of an Inlet or Outlet FTS object.
 */

public class FtsInOutletObject  extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int position;

  /**
   * Create a FtsInOutletObject object.
   */
  
  public FtsInOutletObject(Fts fts, FtsObject parent, String className, int position, int objId)
  {
    super(fts, parent, className, null, className + " " + position, objId);

    this.position = position;
  }

  /** Se the position of the inlet/outlet.
   * Do nothing on fts here; it is the responsability
   * of the subclass
   */

  public void setPosition(int i)
  {
    position = i;
    description = className + " " + position;

    setDirty();
  }

  /** Get the position of the inlet/outlet. */

  public int getPosition()
  {
    return position;
  }
}






