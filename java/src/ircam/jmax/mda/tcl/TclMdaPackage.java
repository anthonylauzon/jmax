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
package ircam.jmax.mda.tcl;

import tcl.lang.*;

import ircam.jmax.*;

/**
 * Tcl Mda extension.
 * Add all the Max Document Architecture TCL commands to the tcl interpreter.
 */

public class TclMdaPackage
{
  /**
   * Add all the commands to the given intepreter.
   *
   * @param interp the TCL interpreter instance
   */

  static public void installPackage()
  {
    Interp interp = MaxApplication.getTclInterp();

    interp.createCommand("jmax",  new JMaxCmd());
  }
}


