/*
 * FtsVersionCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.fts.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * The "version" TCL command, used during load.
 */

class FtsVersionCmd implements Command {

  /**
   * This procedure is invoked to execute a "version" operation in Ermes
   */
  
  public Object CmdProc(Interp interp, CmdArgs ca) {
    
    if (ca.argc < 2) {	//version <string>
      
      throw new EvalException("wrong # args: should be \"" + ca.argv(0)+" <string>");
      
    }

    // Nop, for now
	  
    return "";
    
  }

}



