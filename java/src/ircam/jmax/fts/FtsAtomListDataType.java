package ircam.jmax.fts;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** Class defining the fts patch type (named patch);
 */

public class FtsIntegerVectorDataType extends MaxDataType
{
  public FtsIntegerVectorDataType()
  {
    super("integerVector");

    setPrettyName("Table");
  }

  /** Build an empty instance of FtsPatchData, i.e. an instance
   * corresponding to an empty, new, patcher
   */

  public MaxData newInstance()
  {
    MaxData data;

    return new FtsIntegerVectorData();
  }

  public boolean canMakeNewInstance()
  {
    return true;
  }
}





