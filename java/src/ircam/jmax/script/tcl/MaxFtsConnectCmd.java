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
    if (argv.length != 6 && argv.length != 7)
      throw new TclNumArgsException(interp, 1, argv, "<type> <ftsdir> <ftsname> <socket> <server> <options> [<port>]");

    String serverDir = argv[1].toString();
    String serverName = argv[2].toString();
    String connection = argv[3].toString();
    String host = argv[4].toString();
    String serverOptions = argv[5].toString();

    int port = 0;

    if (argv.length == 7)
      {
	try
	  {
	    port = Integer.parseInt( argv[6].toString());
	  }
	catch( NumberFormatException excp)
	  {
	  }
      }

    MaxApplication.setFts(new Fts( serverDir, serverName, connection, host, serverOptions, 0));
  }
}

