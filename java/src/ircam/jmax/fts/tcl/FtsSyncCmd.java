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

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>sync</b>,
 * used to request a synchronization point with FTS.
 * A syncronization point is a point in time where FTS 
 * have sent all the pending updates, and all the values
 * and pending messages from FTS has been handled by FTS.
 * To be used after requesting printout that may be asynchronious
 * with the TCL printout, for example.
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     sync
 * </code> <p>
 *
 * The TCL command return the number of millisecond needed for
 * the roundtrip and syncronization; this give a good evaluation
 * of the UI latency.
 */

class FtsSyncCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    long before;

    // Retrieve the arguments
    // this call should be substituted by a registration service call

    before = System.currentTimeMillis();

    MaxApplication.getFts().sync();

    interp.setResult((int) (System.currentTimeMillis() - before));
  }
}

