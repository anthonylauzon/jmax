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

package ircam.jmax.editors.patcher.tcl;



import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;

/**
 * Define the commands:
 *
 * patcherMenu add <name> <description> <message>
 * patcherMenu add <submenus> <name> <description> <message>
 * patcherMenu addAndEdit <name> <description> <message>
 * patcherMenu addAndEdit <submenus> <name> <description> <message>
 */

class AbbreviationCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if ((argv.length < 5) || (argv.length > 6))
      throw new TclNumArgsException(interp, 1, argv, "patcherMenu");

    String subcmd = argv[1].toString();

    if (subcmd.equals("add") && argv.length == 5)
      {
	AddPopUp.addAbbreviation(argv[2].toString(), argv[3].toString(), argv[4].toString(), false);
      }
    else if (subcmd.equals("add") && argv.length == 6)
      {
	AddPopUp.addAbbreviation(argv[2].toString(), argv[3].toString(),
				 argv[4].toString(), argv[5].toString(), false);
      }
    else if (subcmd.equals("addAndEdit") && argv.length == 5)
      {
	AddPopUp.addAbbreviation(argv[2].toString(), argv[3].toString(), argv[4].toString(), true);
      }
    else if (subcmd.equals("addAndEdit") && argv.length == 6)
      {
	AddPopUp.addAbbreviation(argv[2].toString(), argv[3].toString(), 
				 argv[4].toString(), argv[5].toString(), false);
      }
  }
}

