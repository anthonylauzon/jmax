package ircam.jmax.fts; 

import java.io.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** A FtsPatchData is the Max Data instance containing an FTS Patch,
 * It have two possible loaders, FtsTpaFileDataHandler and FtsDotPatFileDataHandler
 */

public class FtsPatchData extends MaxTclData
{
  FtsObject patcher;

  public FtsPatchData()
  {
    super(MaxDataType.getTypeByName("patcher"));
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
  
  public void  setContent(Object content)
  {
    patcher = (FtsObject) content;
  }

  /** Save the content (a patcher) as TCL code */

  public void saveContentAsTcl(PrintWriter pw)
  {
    // Sync to fts to be sure we have all the info.

    FtsServer.getServer().syncToFts();

    patcher.saveAsTcl(pw);
  }
}





