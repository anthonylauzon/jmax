package ircam.jmax.fts.tcl;


import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>object</b>,
 * that create an FTS object.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *      object <i>patcher description graphic_data</i>
 * </code>
 */

class FtsObjectCmd implements Command
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
	description = new String(argv[2].toString());

	if (argv.length == 4)
	  object = FtsObject.makeFtsObject(parent, description,
					   new FtsGraphicDescription(new String(argv[3].toString())), false);
	else
	  object = FtsObject.makeFtsObject(parent, description);

	interp.setResult(ReflectObject.newInstance(interp, object));
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<patcher> <description> [<graphic>]");
      }
  }
}


