package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>template</b>,
 * used to declare a <i>tcl</i> template for FTS. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     template <i>name [proc] </i>
 * </code> <p>
 *
 * @see FtsTemplateTable
 */



class FtsTemplateCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length >= 2)
      {
	String name;
	String proc;

	// Retrieve the arguments

	name = new String(argv[1].toString());

	if (argv.length == 3)
	  proc = new String(argv[2].toString());
	else
	  proc = name;

	FtsTemplateTable.add(name, proc);
      }
    else
      {
	throw new TclException(interp, "missing argument; usage: template <name> [<proc>]");
      }
  }
}

