/*
 * MaxPostCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.editors.console.tcl;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.editors.console.*;

/**
 * The "OpenConsole" TCL command in ERMES.
 */

class ConsoleOpenCmd implements Command
{
  /**
   * This procedure is invoked to install the tcl console
   */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    new ConsoleWindow();
  }
}

