//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.script.tcl;


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

