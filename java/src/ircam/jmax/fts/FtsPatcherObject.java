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

public class FtsPatcherObject extends FtsContainerObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  /**
   * Create a FtsPatcherObject object
   */


  public FtsPatcherObject(FtsContainerObject parent, String description, int objId)
  {
    super(parent, "jpatcher", description, objId);
  }


  /**
   * Special method to redefine a patcher without looosing its content 
   */

  public void redefinePatcher(String description)
  {
    Fts.getServer().redefinePatcherObject(this, description);
    Fts.sync();

    this.description = description;

    setDirty();
  }
}










