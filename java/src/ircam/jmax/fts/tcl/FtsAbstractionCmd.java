package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>abstraction</b>,
 * used to declare a <i>.pat</i> abstraction. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     abstraction <i>name path </i>
 * </code> <p>
 *
 * @see FtsAbstractionTable
 */

class FtsAbstractionCmd implements Command
{
  /** Method implementing the TCL command. */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	String name;
	String path;

	// Retrieve the arguments

	name = argv[1].toString();
	path = argv[2].toString();

	FtsAbstractionTable.add(name, path);
      }
    else
      {
	throw new TclException(interp, 
			       "missing argument; usage: abstraction <name> <path>");
      }
  }
}

