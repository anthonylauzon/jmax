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


