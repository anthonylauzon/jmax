package ircam.jmax.fts.tcl;

import ircam.jmax.*;
import tcl.lang.*;

import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>init</b>,
 * that send to a patcher the after-load init message
 * (that run the  loadbang like operations).<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     init <i>obj</i>
 * </code>
 */


class FtsInitCmd implements Command
{
  
  /** Method implementing the TCL command */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	FtsObject obj;

	obj = (FtsObject) ReflectObject.get(interp, argv[1]);

	obj.loaded();
      }
    else
      {
	throw new TclException(interp, "wrong number of args: usage:  init  <patcher>");
      }
  }
}



