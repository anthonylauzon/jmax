/*
 *
 * Copyright (c) 1997 Cornell University.
 * Copyright (c) 1997 Sun Microsystems, Inc.
 * Copyright (c) 1998 Ircam
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 * 
 * SCCS: @(#) SourceCmd.java 1.9 97/10/09 18:32:05
 *
 */

package ircam.jmax.tcl;

import tcl.lang.*;

/*
 * This class implements  a version of the built-in "source"  that do not
 * recursively halt the sourcing of file in case of an error, but just print
 * the error and return.
 */

class MaxSysSourceCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[])  throws   TclException
  {
    String fileName = null;

    if (argv.length == 2)
      {
	fileName = argv[1].toString();
  
	try
	  {
	    interp.evalFile(fileName);
	  }
	catch (TclException e)
	  {
	    System.out.println("TCL Error " + e + " in " + fileName + " : " + interp.getResult());
	  }
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "fileName");
  }
}




