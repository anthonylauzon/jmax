package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>connect</b>,
 * that create a connection between two FTS objects.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     connect <i>from outlet to inlet</i>
 * </code> <p>
 *
 * @see FtsConnection
 */

class FtsConnectionCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 5)
      {
	FtsObject from   = (FtsObject) ReflectObject.get(interp, argv[1]);
	int       outlet = TclInteger.get(interp, argv[2]);
	FtsObject to     = (FtsObject) ReflectObject.get(interp, argv[3]);
	int       inlet  = TclInteger.get(interp, argv[4]);
	FtsConnection c;

	try
	  {
	    c = Fts.makeFtsConnection(from, outlet, to, inlet);
	  }
	catch (FtsException e)
	  {
	    throw new TclException(interp, "Connection error");
	  }

	interp.setResult(ReflectObject.newInstance(interp, c));
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<from> <outlet> <to> <inlet>");
      }
  }
}



