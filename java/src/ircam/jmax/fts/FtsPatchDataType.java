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
    super("patcher");
    setPrettyName("Patcher");
  }

  /** Build an empty instance of FtsPatchData, i.e. an instance
   * corresponding to an empty, new, patcher
   */

  public MaxData newInstance()
  {
    // Build a new FtsObject, a patcher 0 in 0 out

    FtsObject patcher;

    patcher = new FtsPatcherObject(FtsServer.getServer().getRootObject());

    // Put some geometrical property for the window, so we can see it.

    patcher.put("wx", 100);
    patcher.put("wy", 100);
    patcher.put("ww", 500);
    patcher.put("wh", 480);

    FtsPatchData obj = new FtsPatchData();

    obj.setPatcher(patcher);

    return obj;
  }

  public boolean canMakeNewInstance()
  {
    return true;
  }
}





