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

    Mda.installDocumentHandler( new FtsDotPatRemoteDocumentHandler());
    Mda.installDocumentHandler( new FtsBmaxRemoteDocumentHandler());

    // Not yet
    // Mda.installDocumentHandler( new FtsBmaxRemoteDocumentHandler());

    Mda.installDocumentType(new FtsPatcherDocumentType());

    // Install the FTS tcl commands

    ircam.jmax.fts.tcl.TclFtsPackage.installPackage();
  }
}
