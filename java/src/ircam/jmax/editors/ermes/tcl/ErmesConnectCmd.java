/*
 * ErmesConnectCmd.java
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
 * The "connect" TCL command in ERMES.
 */

class ErmesConnectCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    ErmesConnection aConnection = null;

    if (argv.length == 5)
      {
	ErmesSketchPad pad = MaxApplication.getApplication().itsSketchWindow.itsSketchPad;

	aConnection = pad.AddConnectionByInOut((ErmesObject) ReflectObject.get(interp, argv[1]),
					       TclInteger.get(interp, argv[2]),
					       (ErmesObject) ReflectObject.get(interp, argv[3]),
					       TclInteger.get(interp, argv[4]));
	
	interp.setResult(ReflectObject.newInstance(interp, aConnection));
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "srcObj srcOutlet destObj destInlet");
  }
}

