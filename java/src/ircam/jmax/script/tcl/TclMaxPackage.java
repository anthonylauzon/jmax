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

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 */


public class TclMaxPackage 
{
  static public void installPackage(TclInterpreter interp)
  {
    /* TCL generic */

    interp.createCommand("_sysSourceFile", new MaxSysSourceCmd());
    interp.createCommand("sourceFile", new MaxSourceCmd());

    /* FTS */
    interp.createCommand("ftsconnect", new MaxFtsConnectCmd());

    /* browser */
    interp.createCommand("open", new MaxOpenCmd());

    /* Splash screen support */

    interp.createCommand("splash", new MaxSplashCmd());

    /* Declare Icon */

    interp.createCommand("declareIcon", new MaxDeclareIconCmd());
    
    /* hooks */

    interp.createCommand("when", new MaxWhenCmd());
    interp.createCommand("runHooks", new MaxRunHooksCmd());

    interp.createCommand("systemProperty", new MaxSystemPropertyCmd());
    interp.createCommand("setSystemProperty", new MaxSetSystemPropertyCmd());

    interp.createCommand("getMaxVersion", new MaxGetMaxVersionCmd());
    interp.createCommand("quit", new MaxQuitCmd());

    interp.createCommand("setGraphicScaling", new DefineScaleFactorCmd());
  }
}







