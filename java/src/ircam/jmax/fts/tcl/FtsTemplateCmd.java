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

package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>template</b>,
 * used to declare a template. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     template <i>name filename </i>
 * </code> <p>
 *
 */

class FtsTemplateCmd implements Command
{
  /** Method implementing the TCL command. */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	String name;
	String filename;

	// Retrieve the arguments

	name = argv[1].toString();
	filename = argv[2].toString();

	MaxApplication.getFts().templateDeclare(name, filename);
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<name> <filename>");
      }
  }
}

