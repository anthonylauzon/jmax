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
  
  public FtsInletObject(FtsObject parent, int position, int objId)
  {
    super(parent, "inlet", position, objId);

    ninlets = 0;
    noutlets = 1;
  }

  /** Get the number of inlets of the object 
    Overwrite the FtsObject method because inlets
    do not believe to FTS, when it say inlets have
    inlets.
   */

  public int getNumberOfInlets()
  {
    return 0;
  }
}






