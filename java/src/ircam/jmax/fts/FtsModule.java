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

    MaxDataHandler.installDataHandler( new FtsDotPatRemoteDataHandler());

    MaxDataHandler.installDataHandler( new FtsDataHandler()); 

    MaxDataHandler.installDataHandler( new FtsBmaxRemoteDataHandler());

    MaxDataType.installDataType(new FtsPatchDataType());
    MaxDataType.installDataType(new FtsIntegerVectorDataType());
    MaxDataType.installDataType(new FtsAtomListDataType());

    // Install the FTS tcl commands

    ircam.jmax.fts.tcl.TclFtsPackage.installPackage();
  }
}
