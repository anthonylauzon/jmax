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
    FtsIntegerVectorData data;
    
    data = new FtsIntegerVectorData();
    ((FtsIntegerVector) data.getContent()).setSize(128);

    return data;
  }

  public boolean canMakeNewInstance()
  {
    return true;
  }
}





