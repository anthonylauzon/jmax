/*
 * JShowCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.editors.ermes.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;

/**
 * The "show" TCL command, used during load.
 */

class ErmesShowCmd implements Command {
  
  /**
   * This procedure is invoked to execute a "show patcher" operation in Ermes
   * the id is the id of the root object on which we should base this
   * document
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length < 2)
      throw new TclException(interp, "wrong # args: should be show <obj>");

    MaxApplication.getApplication().NewPatcherWindow((FtsObject) ReflectObject.get(interp, argv[1]));
  }
}



