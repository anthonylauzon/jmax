package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>seData</b>,
 * that put an FTS  data object on a FTS object.
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     setData <i>ftsObj ftsDataObj</i>
 * </code> <p>
 *
 */

class FtsSetDataCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	FtsObjectWithData obj  = (FtsObjectWithData) ReflectObject.get(interp, argv[1]);
	FtsDataObject data = (FtsDataObject) ReflectObject.get(interp, argv[2]);

	try
	  {
	    obj.setData(data);

	    interp.setResult(ReflectObject.newInstance(interp, data));
	  }
	catch (FtsException e)
	  {
	    throw new TclException(interp, e.toString());
	  }
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "ftsObj ftsDataObj");
      }
  }
}



