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
 * Class implementing the proxy of an FTS comment object.
 * Do nothing, the actual comment is stored in the comment 
 * property.
 */

public class FtsCommentObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsObject object;
   */

  public FtsCommentObject(FtsObject parent, String description, int objId)
  {
    super(parent, "comment", null, description, objId);

    ninlets = 0;
    noutlets = 0;
  }
}






