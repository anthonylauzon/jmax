/*
 * MaxGetJMaxVersion.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.tcl;


import java.util.*;

import tcl.lang.*;
import ircam.jmax.*;


/**
 * The "getMaxVersion" TCL command.
 * Returns the version string specified in the system-generated class
 * MaxVersion (this class is generate during the install)
 */

class MaxGetMaxVersionCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 1)
      interp.setResult(MaxVersion.getMaxVersion());
    else
      throw new TclNumArgsException(interp, 1, argv, "name");
  }
}

