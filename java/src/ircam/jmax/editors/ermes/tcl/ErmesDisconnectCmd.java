/*

 * ErmesDisconnectCmd.java

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
 * The "disconnect" TCL command in ERMES.
 */

class ErmesDisconnectCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 5)
      { 
    	ErmesSketchPad pad = MaxApplication.getCurrentWindow().itsSketchPad;

	pad.itsHelper.DeleteConnectionByInOut((ErmesObject) ReflectObject.get(interp, argv[1]),
					       TclInteger.get(interp, argv[2]),
					       (ErmesObject) ReflectObject.get(interp, argv[3]),
					       TclInteger.get(interp, argv[4]), true);
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "srcObj srcOutlet destObj destInlet");
      }
  }
}







