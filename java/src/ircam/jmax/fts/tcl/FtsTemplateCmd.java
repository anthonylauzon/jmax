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
 * This class define the TCL Command <b>template</b>,
 * used to declare a template. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     template <i>name filename </i>
 * </code> <p>
 *
 */

class FtsTemplateCmd implements Command
{
  /** Method implementing the TCL command. */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	String name;
	String filename;

	// Retrieve the arguments

	name = argv[1].toString();
	filename = argv[2].toString();

	if (Fts.getServer() != null)
	  {
	    Fts.getServer().sendTemplateDeclare(name, filename);
	    Fts.recomputeErrorObjects();
	  }
	else
	  throw new TclException(interp, "cannot declare a template before startup");
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<name> <filename>");
      }
  }
}

