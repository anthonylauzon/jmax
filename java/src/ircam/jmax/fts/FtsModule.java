//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
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

    Fts.setPatcherDocumentType(new FtsPatcherDocumentType());
    Mda.installDocumentType(Fts.getPatcherDocumentType());

    // Install the FTS tcl commands

    ircam.jmax.fts.tcl.TclFtsPackage.installPackage();

    // Install the meta data object

    FtsRemoteMetaData.install();

    // Install the Integer vector class

    Fts.registerRemoteDataClass("integer_vector", ircam.jmax.fts.FtsIntegerVector.class);

    // Install the Object set class

    Fts.registerRemoteDataClass("object_set_data", ircam.jmax.fts.FtsObjectSet.class);

    // Install the atom list class

    Fts.registerRemoteDataClass("atom_list_data", ircam.jmax.fts.FtsAtomList.class);

    // Install the patcher data class

    Fts.registerRemoteDataClass("patcher_data", ircam.jmax.fts.FtsPatcherData.class);
  }
}
