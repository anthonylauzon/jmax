/*
 * JElementsCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.editors.ermes.tcl;


import tcl.lang.*;

import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;	//?
import ircam.jmax.editors.ermes.*;

/**
 * The "elements" TCL command in ERMES. 
 * Returns the list of all the objects in the sketch.
 */

class ErmesElementsCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    Enumeration e = MaxApplication.getCurrentWindow().itsSketchPad.itsElements.elements();
    ErmesObject aObject = null;
	
    if (argv.length == 1)
      {
	TclObject list;

	list = TclList.newInstance();

	while (e.hasMoreElements())
	  {

	    aObject = (ErmesObject) e.nextElement();

	    TclList.append(interp, list, ReflectObject.newInstance(interp, aObject));
	  }

	interp.setResult(list);
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "");
  }
}



