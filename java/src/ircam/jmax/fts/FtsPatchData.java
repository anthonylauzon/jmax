package ircam.jmax.fts; 

import ircam.jmax.mda.*;

/** A FtsPatchData is the Max Data instance containing an FTS Patch,
 * It have two possible loaders, FtsTpaFileDataHandler and FtsDotPatFileDataHandler
 */

class FtsPatchData extends MaxData
{
  static MaxDataType patchType = null;

  FtsObject patcher;

  static void setFtsPatchType(MaxDataType type)
  {
    patchType = type;
  }

  FtsPatchData()
  {
    super(patchType);
  }

  void setPatcher(FtsObject patcher)
  {
    this.patcher = patcher;
  }

  FtsObject getPatcher()
  {
    return patcher;
  }

  public Object getContent()
  {
    return patcher;
  }
}
