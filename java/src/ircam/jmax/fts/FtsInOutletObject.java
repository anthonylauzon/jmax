//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
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
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
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
  
  public FtsInOutletObject(FtsObject parent, String className, int position, int objId)
  {
    super(parent, className, null, className + " " + position, objId);

    this.position = position;
  }

  public void setPosition(int i)
  {
    position = i;
    description = className + " " + position;

    Fts.getServer().repositionInletObject(this, position);
    setDirty();
  }

  public int getPosition()
  {
    return position;
  }
}






