/*
 * ErmesConnectCmd.java
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
 * The "connect" TCL command in ERMES.
 */

class ErmesConnectCmd implements Command {

  public Object CmdProc(Interp interp, CmdArgs ca) {
    ErmesConnection aConnection = null;
    if (ca.argc != 5) {	// connect, srcId, srcOut, destId, destOut
      throw new EvalException("wrong # args: should be \"" + ca.argv(0) +
			      " srcObjId, srcOutlet, destObjId, destInlet");
    }

    aConnection = MaxApplication.getApplication().itsSketchWindow.itsSketchPad.AddConnectionByInOut(
						ca.intArg(1), ca.intArg(2), ca.intArg(3), ca.intArg(4));
    if (aConnection == null) return "connection error";
    else return "";
  }
}

