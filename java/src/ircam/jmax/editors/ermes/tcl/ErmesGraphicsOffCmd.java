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
 * The "graphicsoff" TCL commands. Every graphic operation will
 * have no effect until the "graphicson" TCL command.
 * Use with care.
 */
class ErmesGraphicsOffCmd implements Command
{
    /**
     * This procedure is invoked to execute a "new patcher operation in Ermes
     */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 1)
      MaxApplication.getApplication().itsSketchWindow.itsSketchPad.itsGraphicsOn = false;
    else
      throw new TclNumArgsException(interp, 1, argv, "");
  }
}

