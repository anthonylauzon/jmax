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

import tcl.lang.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The "when" TCL command in ERMES.
 */

class MaxWhenCmd implements Command {

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      MaxApplication.addHook(new String(argv[1].toString()),
					      new String(argv[2].toString()));
    else
      	throw new TclNumArgsException(interp, 1, argv, "name { body } ");
  }
}

