package ircam.jmax.fts;

import ircam.jmax.mda.*;

/** The Fts module; the init function is called at init time
 *  by jmax, and install module related things
 */

public class FtsModule
{
  static public void initModule()
  {
    // Install the MDA entities

    MaxDataHandler.installDataHandler( new FtsDotPatFileDataHandler()); 
    MaxDataType.installDataType(new FtsPatchDataType());

    // Install the FTS tcl commands

    ircam.jmax.fts.tcl.TclFtsPackage.installPackage();
  }
}
