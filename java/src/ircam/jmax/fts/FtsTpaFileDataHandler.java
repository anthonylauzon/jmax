package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.mda.*;

/** An instance of this data handler can load MaxData from
 *  a tcl file obeyng the "jmax" command conventions
 * 
 * It can be instantiate with a filename extension, so that
 * that extension will be automatically reconized.
 */

public class FtsTpaFileDataHandler extends MaxTclFileDataHandler
{
  String extension;
  
  public FtsTpaFileDataHandler()
  {
    super(".tpa");
  }

  public void saveInstance(MaxData instance) throws MaxDataException
  {
    ///////////// Not yet implemented ???
    /* Open the stream, put the "jmax" header, and then save
       the patcher inside
       */
  }
}
