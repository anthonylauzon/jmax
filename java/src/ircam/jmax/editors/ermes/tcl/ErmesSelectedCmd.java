/*
 * JSelectedCmd.java
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
 * The "selected" TCL command in ERMES.
 * Returns the list of the selected objects.
 */

class ErmesSelectedCmd implements Command 
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 1)
      {
	Enumeration e;
    
	if (MaxApplication.getCurrentWindow() != null)
	  e =  ErmesSketchPad.currentSelection.itsObjects.elements();
	else
	  throw new TclException(interp, "no windows!"); 

	ErmesObject aObject = null;
        
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
