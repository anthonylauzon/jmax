package ircam.jmax.fts;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** Class defining the fts patch type (named patch);
 */

public class FtsAtomListDataType extends MaxDataType
{
  public FtsAtomListDataType()
  {
    super("atomList");

    setPrettyName("Qlist");
  }

  /** Build an empty instance of FtsPatchData, i.e. an instance
   * corresponding to an empty, new, patcher; should we be able to do so ??
   */

  public MaxData newInstance()
  {
    MaxData data;

    return new FtsAtomListData();
  }

  public boolean canMakeNewInstance()
  {
    return true;
  }
}





