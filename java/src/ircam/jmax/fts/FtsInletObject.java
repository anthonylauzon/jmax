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






