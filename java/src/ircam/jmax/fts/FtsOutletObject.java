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

public class FtsOutletObject  extends FtsInOutletObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  int position;

  public FtsOutletObject(FtsObject parent, int position, int objId)
  {
    super(parent, "outlet", position, objId);

    ninlets = 1;
    noutlets = 0;
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






