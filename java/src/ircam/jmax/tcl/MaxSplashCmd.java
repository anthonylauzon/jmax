/*
 * MaxOpenCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.project.*;


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
    if (argv.length == 2)
      {
	String fileName = new String(argv[1].toString());

	new SplashDialog(MaxApplication.getApplication().itsProjectWindow, fileName);    
      }
    else
      {	
      	throw new TclNumArgsException(interp, 1, argv, "<filename>");
      }
  }
}



