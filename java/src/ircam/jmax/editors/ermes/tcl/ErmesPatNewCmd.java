/*
 * NewCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.editors.ermes.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.editors.ermes.*;

/**
 * The "new" TCL command in ERMES. Opens a new, empty patch.
 */

class ErmesPatNewCmd implements Command {

    /**
     * This procedure is invoked to execute a "new patcher operation in Ermes
     */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {  
    if (argv.length > 1)
      throw new TclNumArgsException(interp, 1, argv, "");

    MaxApplication.ObeyCommand(MaxApplication.NEW_COMMAND);

    // It should get back the new patcher, and return it for future commands !!!
  }
}

