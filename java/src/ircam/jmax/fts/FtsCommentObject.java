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


  public void setComment(String comment)
  {
    description = comment;
    Fts.getServer().sendSetMessage(this, description);
    setDirty();
  }
}






