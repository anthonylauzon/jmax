//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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


