//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts.tcl;

import tcl.lang.*;
import ircam.jmax.*;

/**
 * Tcl FTS extension.
 * Add all the application layer TCL commands to the tcl interpreter.
 * 
 * @see FtsPatcherCmd
 * @see FtsObjectCmd
 * @see FtsDeclareCmd
 * @see FtsConnectionCmd
 * @see FtsInitCmd
 * @see FtsSyncCmd
 * @see FtsAbstrationCmd
 * @see FtsHelpPatchCmd
 * @see FtsReferenceURLCmd
 * @see FtsMessCmd
 * @see FtsUcsCmd
 * @see StatsCmd
 * @see FtsClassNameCmd
 * @see FtsSetPropertyCmd
 * @see FtsIntegerVectorCmd
 */

public class TclFtsPackage
{
  /**
   * Add all the commands to the given intepreter.
   *
   * @param interp the TCL interpreter instance
   */

  static public void installPackage()
  {
    Interp interp = MaxApplication.getTclInterp();

    // Abstraction and templates declaration, help and blah blah

    interp.createCommand("abstraction",  new FtsAbstractionCmd());
    interp.createCommand("abstractionPath",  new FtsAbstractionPathCmd());
    interp.createCommand("template",  new FtsTemplateCmd());
    interp.createCommand("templatePath",  new FtsTemplatePathCmd());

    interp.createCommand("helpPatch",  new FtsHelpPatchCmd());
    interp.createCommand("helpSummary",  new FtsHelpSummaryCmd());
    interp.createCommand("referenceURL",  new FtsReferenceURLCmd());

    // UCS

    interp.createCommand("ucs", new FtsUcsCmd());

    // Sync

    interp.createCommand("sync", new FtsSyncCmd());
  }
}


