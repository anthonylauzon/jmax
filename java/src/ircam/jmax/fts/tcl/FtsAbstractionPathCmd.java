//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
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
 *     abstractionPath <i>path </i>
 * </code> <p>
 *
 */

class FtsAbstractionPathCmd implements Command
{
  /** Method implementing the TCL command. */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	String path;

	// Retrieve the arguments

	path = argv[1].toString();

	Fts.getServer().sendAbstractionDeclarePath(path);
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<path>");
      }
  }
}

