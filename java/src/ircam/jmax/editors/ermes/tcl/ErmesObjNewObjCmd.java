/*
 * NewObjCmd.java
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
 * The "newobj" TCL command in ERMES. Create a new object on the sketch, 
 * given its name, its position, its arguments.
 */

class ErmesObjNewObjCmd implements Command
{
    /**
     * This procedure is invoked to execute a "new object" operation in Ermes
     */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    ErmesObject aObject = null;

    if (argv.length >= 4)
      {
	ErmesSketchPad pad;

	pad = MaxApplication.getApplication().itsSketchWindow.itsSketchPad;

	if (argv.length == 4)
	  {
	    //all the modules except the externals and messages
	    aObject  = pad.AddObjectByName(new String(argv[1].toString()), TclInteger.get(interp, argv[2]),
					   TclInteger.get(interp, argv[3]), "");
	  }
	else if (argv.length == 5)
	  {
	    // externals and messages

	    aObject  = pad.AddObjectByName(new String(argv[1].toString()), TclInteger.get(interp, argv[2]),
					   TclInteger.get(interp, argv[3]), new String(argv[4].toString()));
	  }

	if (aObject != null)
	  interp.setResult(ReflectObject.newInstance(interp, aObject));
	else 
	  throw new TclException(interp, "error in object creation");
      }
    else
      throw new TclException(interp, "wrong number of arguments: usage: new <name> <x> <y>");
  }
}





