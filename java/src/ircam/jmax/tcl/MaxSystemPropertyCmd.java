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


import java.io.*;
import java.util.*;
import tcl.lang.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * The "systemProperty" TCL command.
 */

class MaxSystemPropertyCmd implements Command
{
  /**
   * This procedure is invoked to get a property
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	String property = MaxApplication.getProperty(new String(argv[1].toString()));

	/* If the property is not defined, don't raise an error, return the empty string, 
	   so a TCL script can test the existence of a systemProperty.
	   Actually, we should return an errror, because the scripts can check errors with
	   the "catch" primitive
	   */

	if (property == null) 
	  interp.setResult("");
	else
	  interp.setResult(property);
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "property");
  }
}



