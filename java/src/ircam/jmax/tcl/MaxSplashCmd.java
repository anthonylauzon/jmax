//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.tcl;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;


/**
 * The  "splash <filename>" TCL command.
 */

class MaxSplashCmd implements Command
{
  /**
   * This procedure is invoked to open a "splash" screen
   * It is used by jmax itself, but can be used by single packages also
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    String Version;
    if (argv.length == 3) Version = new String(argv[2].toString());
    else if (argv.length == 2) Version = new String("");
    else {	
      throw new TclNumArgsException(interp, 2, argv, "<filename> <version number>");
    }
    
    String fileName = new String(argv[1].toString());
    //it's going to become a window instead of a dialog (no assumptions 
    //shoul be made on the presence of a frame on top of which "mount"
    //the splash)
    new SplashDialog(fileName, Version);    
  }
}



