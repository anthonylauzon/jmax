/*
 * MaxQuitCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.tcl;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import cornell.Jacl.*;

/**
 * The "quit" TCL command
 */

class MaxQuitCmd implements Command {

  
  
  /**
    
   * This procedure is invoked to shutdown the system
   */
  
  public Object CmdProc(Interp interp, CmdArgs ca) {
    
    if (ca.argc != 1) {	//please write only "quit"
      
      throw new EvalException("wrong # args: should be \"");
      
    }

    MaxApplication.getApplication().ObeyCommand(MaxApplication.QUIT_APPLICATION);
	  
    return "";
    
  }

}



