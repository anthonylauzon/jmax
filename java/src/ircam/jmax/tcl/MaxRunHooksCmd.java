/*

 * MaxRunHooksCmd.java

 *

 * Copyright (c) 1997 IRCAM.

 *

 */



package ircam.jmax.tcl;


import java.util.*;

import tcl.lang.*;
import ircam.jmax.*;


/**
 * The "when" TCL command in ERMES.
 */

class MaxRunHooksCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      MaxApplication.getApplication().runHooks(new String(argv[1].toString()));
    else
      throw new TclException(interp, "usage: runHooks name");
  }
}

