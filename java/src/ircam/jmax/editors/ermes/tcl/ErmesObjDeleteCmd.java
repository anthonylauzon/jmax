/*

 * ErmesObjDeleteCmd.java

 *

 * Copyright (c) 1997 IRCAM.

 *

 */



package ircam.jmax.editors.ermes.tcl;



import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.editors.ermes.*;


/**
 * The "delete" TCL command in ERMES.
 */

class ErmesObjDeleteCmd implements Command
{

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
      ErmesObject aObject = null;
      Object oneObject = null;
     
      for (int i = 1; i < argv.length; i++)
	{
	  aObject = (ErmesObject) ReflectObject.get(interp, argv[i]);
	  aObject.itsSketchPad.itsHelper.DeleteObject(aObject);
	}
  }
}





