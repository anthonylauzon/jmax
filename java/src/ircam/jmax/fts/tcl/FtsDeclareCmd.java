package ircam.jmax.fts.tcl;


import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>declare</b>,
 * that create a FTS declaration.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *    declare <i>description</i>
 * </code>
 */


class FtsDeclareCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	FtsObject object;
	FtsContainerObject parent;
	String    description;

	// Retrieve the arguments

	parent = (FtsContainerObject) FtsContainerObject.containerStack.peek();
	description = argv[1].toString();

	try
	  {
	    object = Fts.makeFtsObject(parent, description);
	  }
	catch (FtsException e)
	  {
	    // Should actually post an error to the FTS queue ??
	    throw new TclException(interp, e.toString());
	  }	

	interp.setResult(ReflectObject.newInstance(interp, object));
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<description> ");
      }
  }
}







