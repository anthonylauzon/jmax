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
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/**
 * Class implementing the proxy of an FTS object.
 * It deals with: Object creation/deletion, connections
 * object properties, class and instance information,
 * FTS instantiation 
 */

public class FtsPatcherObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsPatcherObject object
   */


  public FtsPatcherObject(Fts fts, FtsObject parent, String variableName, String description, int objId)
  {
    super(fts, parent, "jpatcher", variableName, description, objId);
  }


  /**
   * Special method to redefine a patcher without looosing its content 
   * To be changed according to the new syntax. !!!!!!
   */

  public void redefinePatcher(String description)
  {
    fts.getServer().redefinePatcherObject(this, description);
    fts.sync();

    this.description = description;

    setDirty();
  }
}










