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






