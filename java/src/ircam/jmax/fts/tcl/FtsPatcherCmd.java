package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>_patcher</b>,
 * used by the tcl function patcher, that create a FTS patcher.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *  patcher <i>properties body</i>
 * </code> <p>
 *
 * The first form (without inlets, patcher and graphic data) 
 * correspond to a root patcher.
 */



class FtsPatcherCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3) 
      {
	Vector args;
	FtsPatcherObject object;
	FtsContainerObject parent;
	TclObject properties;
	TclObject body;

	if (FtsContainerObject.containerStack.empty())
	  {
	    parent     = MaxApplication.getFtsServer().getRootObject();
	    MaxApplication.getFtsServer().setFlushing(false);
	  }
	else
	  parent     = (FtsContainerObject) FtsContainerObject.containerStack.peek();

	properties = argv[1];
	body       = argv[2];

	try
	  {
	    object = new FtsPatcherObject(parent);

	    object.parseTclProperties(interp, properties);
	    object.updateFtsObject(); //neede to update ins/outs and name
	    object.eval(interp, body);

	    // Run the after load init of the patcher

	    object.loaded();

	    // Set back the server to flushing if we are at the top of 
	    // the stack

	    if (FtsContainerObject.containerStack.empty())
	      MaxApplication.getFtsServer().setFlushing(true);

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
	throw new TclNumArgsException(interp, 1, argv, "<properties> <body>");
      }
  }
}


