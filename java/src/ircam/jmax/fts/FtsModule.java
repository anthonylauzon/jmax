package ircam.jmax.fts;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** The Fts module; the init function is called at init time
 *  by jmax, and install module related things
 */

public class FtsModule
{
  static public void initModule()
  {
    // Install the MDA entities

    if (MaxApplication.getProperty("turbo", "off").equals("on"))
      {
	MaxDataHandler.installDataHandler( new FtsDotPatRemoteDataHandler());
      }
    else
      MaxDataHandler.installDataHandler( new FtsDotPatFileDataHandler()); 

    MaxDataHandler.installDataHandler( new FtsDataHandler()); 

    MaxDataType.installDataType(new FtsPatchDataType());
    MaxDataType.installDataType(new FtsIntegerVectorDataType());

    // Install the FTS tcl commands

    ircam.jmax.fts.tcl.TclFtsPackage.installPackage();
  }
}
