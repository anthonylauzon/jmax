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
  public FtsTpaFileDataHandler()
  {
    super(".tpa");
  }
}
