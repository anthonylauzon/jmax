/*
 * MaxOpenCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.tcl;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;


/**
 * The  "splash <filename>" TCL command.
 */

class MaxSplashCmd implements Command
{
  /**
   * This procedure is invoked to open a "splash" screen
   * It is used by jmax itself, but can be used by single packages also
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    String Version;
    if (argv.length == 3) Version = new String(argv[2].toString());
    else if (argv.length == 2) Version = new String("");
    else {	
      throw new TclNumArgsException(interp, 2, argv, "<filename> <version number>");
    }
    
    String fileName = new String(argv[1].toString());
    //it's going to become a window instead of a dialog (no assumptions 
    //shoul be made on the presence of a frame on top of which "mount"
    //the splash)
    new SplashDialog(fileName, Version);    
  }
}



