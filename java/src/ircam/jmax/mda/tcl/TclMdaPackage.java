package ircam.jmax.mda.tcl;

import tcl.lang.*;

/**
 * Tcl Mda extension.
 * Add all the Max Data Architecture TCL commands to the tcl interpreter.
 */

public class TclMdaPackage
{
  /**
   * Add all the commands to the given intepreter.
   *
   * @param interp the TCL interpreter instance
   */

  static public void installPackage(Interp interp)
  {
    interp.createCommand("installEditorFor",  new InstallEditorForCmd());
    interp.createCommand("jmax",  new JMaxCmd());
  }
}


