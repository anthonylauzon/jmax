package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>helpSummary</b>,
 * that register in the helpdata base a named help summary.
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     helpSummary <i>name filename </i>
 * </code>
 */

class FtsHelpSummaryCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	String name;
	String patch;

	// Retrieve the arguments

	name  = new String(argv[1].toString());
	patch = new String(argv[2].toString());

	FtsHelpPatchTable.addSummary(name, patch);
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<name> <filename>");
      }
  }
}










