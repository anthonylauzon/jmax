/*
 * ErmesGraphicsOnCmd.java
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
 * The "graphicson" TCL commands. Reset the normal state of the graphic 
 * after a "graphicoff" TCL command.
 * Use with care.
 */

class ErmesGraphicsOnCmd implements Command
{
  /**
   * This procedure is invoked to execute a "new patcher operation in Ermes
   */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 1)
      {
	MaxApplication.getCurrentWindow().itsSketchPad.itsGraphicsOn = true;
	MaxApplication.getCurrentWindow().itsSketchPad.repaint();
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "");
  }
}

