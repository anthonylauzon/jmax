package ircam.jmax.fts.tcl;


import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>className</b>,
 * that get the class name of a FTS object. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *  className <i>obj</i>
 * </code> <p>
 *
 * @see FtsObject#getClassName
 */

class FtsClassNameCmd implements Command
{
  /** Method implementing the TCL command. */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	FtsObject obj = (FtsObject) ReflectObject.get(interp, argv[1]);

	interp.setResult(obj.getClassName());
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<obj>");
      }
  }
}

