/*
 * MaxWhenCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.tcl;

import tcl.lang.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The "when" TCL command in ERMES.
 */

class MaxWhenCmd implements Command {

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      MaxApplication.getApplication().addHook(new String(argv[1].toString()),
					      new String(argv[2].toString()));
    else
      	throw new TclNumArgsException(interp, 1, argv, "name { body } ");
  }
}

