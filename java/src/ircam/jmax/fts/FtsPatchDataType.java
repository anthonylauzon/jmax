package ircam.jmax.fts;

import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** Class defining the fts patch type (named patch);
 */

public class FtsPatchDataType extends MaxDataType
{
  public FtsPatchDataType()
  {
    super("patch");
  }

  /** Build an empty instance of FtsPatchData, i.e. an instance
   * corresponding to an empty, new, patcher
   */

  public MaxData newInstance()
  {
    // Build a new FtsObject, a patcher 0 in 0 out

    FtsObject patcher;

    patcher = new FtsPatcherObject(MaxApplication.getFtsServer().getRootObject());

    // Put some geometrical property for the window, so we can see it.

    patcher.put("win.pos.x", 100);
    patcher.put("win.pos.y", 100);
    patcher.put("win.size.w", 300);
    patcher.put("win.size.h", 300);

    FtsPatchData obj = new FtsPatchData();

    obj.setPatcher(patcher);

    return obj;
  }
}




