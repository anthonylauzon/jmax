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

package ircam.jmax.tcl;


import java.io.*;
import java.util.*;
import tcl.lang.*;
import ircam.jmax.*;


/**
 * The "setSystemProperty" TCL command.
 */

class MaxSetSystemPropertyCmd implements Command
{
  /**
   * This procedure is invoked to set a property
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	String ret = MaxApplication.setProperty( argv[1].toString(), argv[2].toString());

	if (ret == null) 
	  interp.setResult("");
	else
	  interp.setResult( ret);
      }
    else
      throw new TclNumArgsException(interp, 2, argv, "<property> <value>");
  }
}



