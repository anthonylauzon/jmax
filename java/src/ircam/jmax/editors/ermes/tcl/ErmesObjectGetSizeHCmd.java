/*
 * JSetObjPos.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */
package ircam.jmax.editors.ermes.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;  //?
import ircam.jmax.editors.ermes.*;

/**
 * The "ObjGetSizeH" TCL command in ERMES.
 */

class ErmesObjectGetSizeHCmd implements Command 
{

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {  
    if (argv.length == 2)
      {
	ErmesObject aObject;

	aObject = (ErmesObject) ReflectObject.get(interp, argv[1]);

	interp.setResult(TclInteger.newInstance(aObject.currentRect.height));
      }
    else
      {
      throw new TclNumArgsException(interp, 1, argv, "<obj>");
      }
  }
}
