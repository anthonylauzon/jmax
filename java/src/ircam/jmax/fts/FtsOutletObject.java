//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * Proxy of an Outlet FTS object.
 */

public class FtsOutletObject  extends FtsInOutletObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int position;

  /**
   * Create a FtsOutletObject object.
   */

  public FtsOutletObject(Fts fts, FtsObject parent, int position, int objId)
  {
    super(fts, parent, "outlet", position, objId);

    ninlets = 1;
    noutlets = 0;
  }

  /** Set the position. Tell the server about the new position */

  public void setPosition(int i)
  {
    super.setPosition(i);

    fts.getServer().repositionOutletObject(this, i);
  }

  /** Get the number of outlets of the object 
    Overwrite the FtsObject method because inlets
    do not believe to FTS, when it say outlets have
    outlets.
    */

  public int getNumberOfOutlets()
  {
    return 0;
  }
}






