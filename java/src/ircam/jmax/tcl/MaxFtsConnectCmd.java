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
import ircam.jmax.fts.*;

/**
 * The "FtsConnect" TCL command.
 * Should be moved to the ircam.jmax.fts.tcl package.
 */

class MaxFtsConnectCmd implements Command
{
  /**
   * This procedure is invoked to establish an FTS connection
   */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 6)
      MaxApplication.setFts(new Fts(new String(argv[1].toString()),
				    new String(argv[2].toString()),
				    new String(argv[3].toString()),
				    new String(argv[4].toString()),
				    new String(argv[5].toString())));
    else if (argv.length == 5)
      MaxApplication.setFts(new Fts(new String(argv[1].toString()),
						 new String(argv[2].toString()),
						 new String(argv[3].toString()),
						 new String(argv[4].toString()),
						 null));
    else
      throw new TclNumArgsException(interp, 1, argv, "<type> <ftsdir> <ftsname> <socket> <server> <port>");
  }
}

