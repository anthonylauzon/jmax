/*

 * ErmesDisconnectCmd.java

 *

 * Copyright (c) 1997 IRCAM.

 *

 */



package ircam.jmax.editors.ermes.tcl;


import cornell.Jacl.*;
import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.editors.ermes.*;

/**
 * The "disconnect" TCL command in ERMES.
 */

class ErmesDisconnectCmd implements Command {

  public Object CmdProc(Interp interp, CmdArgs ca) {
 
    if (ca.argc != 5) {	// connect, srcId, srcOut, destId, destOut
      throw new EvalException("wrong # args: should be \"" + ca.argv(0) +
			      " srcObjId, srcOutlet, destObjId, destInlet");
    }
    
    boolean ok = MaxApplication.getApplication().itsSketchWindow.itsSketchPad.itsHelper.DeleteConnectionByInOut(
					 ca.intArg(1), ca.intArg(2), ca.intArg(3), ca.intArg(4));
    if(!ok)return "no such connection";
    else return "";
  }
}







