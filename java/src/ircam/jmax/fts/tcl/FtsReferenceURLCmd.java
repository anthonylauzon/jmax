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

	name = new String(argv[1].toString());
	url  = new String(argv[2].toString());

	FtsReferenceURLTable.add(name, url);
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<name> <url>");
      }
  }
}

