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
 *      inlet <i> <position> <properties> <i>
 * </code>
 */

class FtsInletCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	FtsObject object;
	FtsContainerObject parent;
	int position;
	TclObject properties;

	try
	  {
	    // Retrieve the arguments
	    parent = (FtsContainerObject) FtsPatcherCmd.patcherStack.peek();
	    position    = TclInteger.get(interp, argv[1]);
	    properties = argv[2];

	    object = new FtsInletObject(parent, position);
	    object.parseTclProperties(interp, properties);

	    interp.setResult(ReflectObject.newInstance(interp, object));
	  }
	catch (FtsException e)
	  {
	    // Should actually post an error to the FTS queue ??
	    throw new TclException(interp, e.toString());
	  }
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<position> <properties>");
      }
  }
}


