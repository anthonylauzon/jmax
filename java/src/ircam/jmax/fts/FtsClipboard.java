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
import ircam.jmax.utils.*;

/**
 * Implement an interface to the FTS the clipboard.
 * Support two methods: copy from a FtsSelection, and paste to an FtsContainer 
 */

public class FtsClipboard  extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a Fts clipboard;
   */

  boolean empty = true; // Empty only before its first copy

  int copyCount = 0;

  protected  FtsClipboard(FtsObject parent, String className, String description, int objId)
  {
    super(parent, className, null, description, objId);
  }

  public void copy(FtsSelection sel)
  {
    empty = false;
    copyCount++;
    Fts.getServer().sendObjectMessage(this, -1, "copy", sel);
  }

  public void paste(FtsObject patcher)
  {
    Fts.getServer().sendObjectMessage(this, -1, "paste", patcher);
  }

  public int getCopyCount()
  {
    return copyCount;
  }

  public boolean isEmpty()
  {
    return empty;
  }
}






