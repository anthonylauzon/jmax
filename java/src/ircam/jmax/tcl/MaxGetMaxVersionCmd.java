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
 * The "getMaxVersion" TCL command.
 * Returns the version string specified in the system-generated class
 * MaxVersion (this class is generate during the install)
 */

class MaxGetMaxVersionCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 1)
      interp.setResult((String) MaxApplication.getProperty("jmaxVersion"));
    else
      throw new TclNumArgsException(interp, 1, argv, "name");
  }
}




