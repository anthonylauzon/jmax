 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** The Fts module; the init function is called at init time
 *  by jmax, and install module related things.
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

    // Install the Integer vector class
    Fts.registerRemoteDataClass("ivec", ircam.jmax.fts.FtsIntegerVector.class);
    Fts.registerRemoteDataClass("fvec", ircam.jmax.fts.FtsIntegerVector.class);

    // Install the Object set class
    Fts.registerRemoteDataClass("object_set_data", ircam.jmax.fts.FtsObjectSet.class);

    // Install the atom list class
    Fts.registerRemoteDataClass("atom_list_data", ircam.jmax.fts.FtsAtomList.class);

    // Install the patcher data class
    Fts.registerRemoteDataClass("patcher_data", ircam.jmax.fts.FtsPatcherData.class);
  }
}
