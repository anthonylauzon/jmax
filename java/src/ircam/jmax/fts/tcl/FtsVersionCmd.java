/*
 * FtsVersionCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * The "version" TCL command, used during load.
 */


class FtsVersionCmd implements Command {
  /**
   * This procedure is invoked to execute a "version" operation in Ermes
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    // Nop, for now
  }
}



