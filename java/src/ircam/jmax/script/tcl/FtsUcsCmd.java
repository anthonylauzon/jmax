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
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>ucs</b>,
 * used to execute an FTS ucs command. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     ucs <i>args</i>
 * </code>
 */


class FtsUcsCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length >  1)
      {
	MaxVector args = new MaxVector();

	for (int i = 1; i < argv.length; i++)
	  {
	    try
	      {
		args.addElement(new Integer(TclInteger.get(interp, argv[i])));
	      }
	    catch (TclException e)
	      {
		try
		  {
		    args.addElement(new Float(TclDouble.get(interp, argv[i])));
		  }
		catch (TclException e2)
		  {
		    args.addElement(new String(argv[i].toString()));
		  }
	      }
	  }

	MaxApplication.getFts().ucsCommand(args);
	interp.resetResult();
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "[args]*");
      }
  }
}


