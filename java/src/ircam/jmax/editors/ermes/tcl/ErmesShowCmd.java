/*
 * JShowCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.editors.ermes.tcl;

import cornell.Jacl.*;
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
  
  public Object CmdProc(Interp interp, CmdArgs ca) {
    
    if (ca.argc < 2) {	//patcher_document <id>
      
      throw new EvalException("wrong # args: should be \"" + ca.argv(0)+" <id>");
      
    }
    MaxApplication.getApplication().NewPatcherWindow(MaxApplication.getFtsServer().getObjectByFtsId(ca.intArg(1)));
	  
    
    return "";
    
  }

}



