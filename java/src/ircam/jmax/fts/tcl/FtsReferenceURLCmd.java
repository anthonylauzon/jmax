package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.fts.*;
/**
 * This class define the TCL Command <b>referenceURL</b>,
 * that register in the documentation data base an reference url
 * for a given FTS class.
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     referenceURL <i>className url </i>
 * </code>
 */

class FtsReferenceURLCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	String name;
	String url;

	// Retrieve the arguments

	name = argv[1].toString();
	url  = argv[2].toString();

	FtsReferenceURLTable.add(name, url);
      }
    else
      {
	throw new TclException(interp, "missing argument; usage: referenceURL <name> <url>");
      }
  }
}

