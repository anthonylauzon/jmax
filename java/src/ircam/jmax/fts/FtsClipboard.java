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

  protected  FtsClipboard(FtsObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);
  }

  public void copy(FtsSelection sel)
  {
    Fts.getServer().sendObjectMessage(this, -1, "copy", sel);
  }

  public void paste(FtsObject patcher)
  {
    Fts.getServer().sendObjectMessage(this, -1, "paste", patcher);
  }
}






