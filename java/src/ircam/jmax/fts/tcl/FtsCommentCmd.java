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
 *      _comment <i> <parent> <description> <properties> <i>
 * </code>
 */

class FtsCommentCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 4)
      {
	FtsObject object;
	FtsContainerObject parent;
	String    description;
	TclObject properties;

	try
	  {
	    // Retrieve the arguments

	    parent = (FtsContainerObject) ReflectObject.get(interp, argv[1]);
	    description = argv[2].toString();
	    properties = argv[3];

	    object = new FtsCommentObject(parent, description);
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
	throw new TclNumArgsException(interp, 1, argv, "<object> <description> <properties>");
      }
  }
}


