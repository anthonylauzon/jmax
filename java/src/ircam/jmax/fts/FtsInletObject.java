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
 * Proxy of an Inlet FTS object.
 */

public class FtsInletObject  extends FtsInOutletObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsInletObject object.
   */
  
  public FtsInletObject(Fts fts, FtsObject parent, int position, int objId)
  {
    super(fts, parent, "inlet", position, objId);

    ninlets = 0;
    noutlets = 1;
  }


  /** Set the position. Tell the server about the new position */

  public void setPosition(int i)
  {
    super.setPosition(i);

    fts.getServer().repositionInletObject(this, i);
  }

  /** Get the number of inlets of the object.
    Overwrite the FtsObject method because inlets
    do not believe to FTS, when it say inlets have
    inlets.
   */

  public int getNumberOfInlets()
  {
    return 0;
  }
}






