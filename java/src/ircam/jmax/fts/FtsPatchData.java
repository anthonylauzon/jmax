package ircam.jmax.fts; 

import java.io.*;
import ircam.jmax.mda.*;

/** A FtsPatchData is the Max Data instance containing an FTS Patch,
 * It have two possible loaders, FtsTpaFileDataHandler and FtsDotPatFileDataHandler
 */

class FtsPatchData extends MaxTclData
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

  /** Get the content (a patcher) as TCL code */

  public Object getContent()
  {
    return patcher;
  }
  
  protected void  setContent(Object content)
  {
    patcher = (FtsObject) content;
  }

  /** Save the content (a patcher) as TCL code */

  public void saveContentAsTcl(PrintWriter pw)
  {
    // patcher.saveAsTcl(pw);
  }
}

