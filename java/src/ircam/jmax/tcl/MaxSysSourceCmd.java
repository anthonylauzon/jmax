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

/*
 * This class implements  a version of the built-in "source"  that do not
 * recursively halt the sourcing of file in case of an error, but just print
 * the error and return.
 */

class MaxSysSourceCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[])  throws   TclException
  {
    String fileName = null;

    if (argv.length == 2)
      {
	fileName = argv[1].toString();
  
	try
	  {
	    interp.evalFile(fileName);
	  }
	catch (TclException e)
	  {
	    System.out.println("TCL Error :" + interp.getResult());
	  }
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "fileName");
  }
}




