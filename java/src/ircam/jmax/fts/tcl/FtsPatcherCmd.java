package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>patcher</b>,
 * that create a FTS patcher.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *  patcher <i>properties body</i>
 * </code> <p>
 *
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
	    parent     = Fts.getServer().getRootObject();
	  }
	else
	  parent     = (FtsContainerObject) FtsContainerObject.containerStack.peek();

	properties = argv[1];
	body       = argv[2];

	try
	  {
	    object = (FtsPatcherObject) Fts.makeFtsObject(parent, "patcher", "unnamed 0 0");

	    object.parseTclProperties(interp, properties);
	    // @@@ BROKEN BY THE EXPRESSION THINGS FOR PATCHERS !!!
	    // object.updateFtsObject(); //neede to update ins/outs and name
	    object.setDownloaded();
	    object.eval(interp, body);
	    
	    // Set back the server to flushing if we are at the top of 
	    // the stack

	    if (FtsContainerObject.containerStack.empty())
	      {
		// Run the after load init of the top level patcher 
		object.loaded();
	      }

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









