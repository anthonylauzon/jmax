package ircam.jmax.fts; 

import java.io.*;


import ircam.jmax.*;
import ircam.jmax.mda.*;

/** A FtsPatcherDocument is the Max Document containing an FTS Patch.
 */

public class FtsPatcherDocument extends MaxDocument 
{
  public FtsPatcherDocument()
  {
    super(Mda.getDocumentTypeByName("patcher"));
  }

  public void setRootData(MaxData data)
  {
    super.setRootData(data);
    ((FtsPatcherData) data).getContainerObject().setDocument(this);
  }

  public void dispose()
  {
    super.dispose();

    FtsObject patcher;

    patcher = ((FtsPatcherData) getRootData()).getContainerObject();
    patcher.delete();
  }
}






