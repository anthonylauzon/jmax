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
	FtsContainerObject parent;
	String    description;

	// Retrieve the arguments

	parent = (FtsContainerObject) ReflectObject.get(interp, argv[1]);
	description = new String(argv[2].toString());

	// object = FtsObject.makeFtsObject(parent, description);

// 	if (argv.length == 4)
// 	  object.setGraphicDescription(new FtsGraphicDescription(argv[3].toString()));

// 	interp.setResult(ReflectObject.newInstance(interp, object));
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<parent> <description> [<graphic>]");
      }
  }
}







