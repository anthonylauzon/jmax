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
import ircam.jmax.editors.console.*;


/**

 * The generic "Open <filename>" TCL command.

 */

class MaxOpenCmd implements Command {

  
  
  /**
    
   * This procedure is invoked to execute a "Open" operation in Ermes
   * It calls the generic OpenFile procedure of the project manager
   * It is able to load everything that can be opened by the "open" menu
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	//ProjectWindow aProjectWindow = MaxApplication.itsProjectWindow;

	// Should call MaxApplication.Open, not the project !!!

	MaxApplication.GetConsoleWindow().OpenFile(new File(new String(argv[1].toString())));
	// Should return the document produced !!!
      }
    else
      {	
      	throw new TclNumArgsException(interp, 1, argv, "<filename>");
      }
  }
}



