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
 *      outlet <i> <position> <properties> <i>
 * </code>
 */

class FtsOutletCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	FtsObject object;
	FtsContainerObject parent;
	String position;
	TclObject properties;

	try
	  {
	    // Retrieve the arguments

	    parent = (FtsContainerObject) FtsContainerObject.containerStack.peek();
	    position    = argv[1].toString();
	    properties = argv[2];

	    object = Fts.makeFtsObject(parent, "outlet", position);
	    object.parseTclProperties(interp, properties);

	    interp.setResult(ReflectObject.newInstance(interp, object));
	  }
	catch (FtsException e)
	  {
	    // Should actually post an error to the FTS error queue ??
	    throw new TclException(interp, e.toString());
	  }
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<position> <properties>");
      }
  }
}


