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

  public void setRootData(MaxData patcher)
  {
    super.setRootData(patcher);
    ((FtsContainerObject) patcher).setDocument(this);
  }

  public void dispose()
  {
    super.dispose();

    FtsContainerObject patcher;

    patcher = (FtsContainerObject) getRootData();
    patcher.delete();
  }
}






