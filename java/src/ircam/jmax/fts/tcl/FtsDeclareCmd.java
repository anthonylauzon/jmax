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
 *    declare <i>patcher description graphic_data</i>
 * </code>
 */


class FtsDeclareCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if ((argv.length >= 3) && (argv.length <= 4))
      {
	FtsObject object;
	FtsObject parent;
	String    description;

	// Retrieve the arguments

	parent = (FtsObject) ReflectObject.get(interp, argv[1]);
	description = argv[2].toString();

	if (argv.length == 4)
	  object = FtsObject.makeFtsObject(parent, description,
					   new FtsGraphicDescription(argv[3].toString()), true);
	else
	  object = FtsObject.makeFtsObject(parent, description, null, true);

	interp.setResult(ReflectObject.newInstance(interp, object));
      }
    else
      {
	throw new TclException(interp, "missing argument; usage: declare <parent> <description> [<graphic>]");
      }
  }
}







