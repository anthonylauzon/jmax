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
	    System.out.println("TCL Error :" + interp.getResult());
	  }
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "fileName");
  }
}




