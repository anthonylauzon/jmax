/*
 * MaxQuitCmd.java
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
 * The "quit" TCL command
 */

class MaxQuitCmd implements Command
{
  /**
   * This procedure is invoked to shutdown the system
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 1)
      MaxApplication.getApplication().ObeyCommand(MaxApplication.QUIT_APPLICATION);
    else
      throw new TclException(interp, "wrong number of arguments: usage: quit");
  }
}



