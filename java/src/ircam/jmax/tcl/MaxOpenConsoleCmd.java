/*
 * MaxPostCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;
import ircam.jmax.*;

/**
 * The "OpenConsole" TCL command in ERMES.
 */

class MaxOpenConsoleCmd implements Command
{
  /**
   * This procedure is invoked to install the tcl console
   */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    MaxApplication.getApplication().makeMaxConsole();
  }
}

