//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//


package ircam.jmax.tcl;


import java.util.*;

import tcl.lang.*;
import ircam.jmax.*;


/**
 * The "when" TCL command in ERMES.
 */

class MaxRunHooksCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      MaxApplication.runHooks(new String(argv[1].toString()));
    else
      throw new TclNumArgsException(interp, 1, argv, "name");
  }
}

