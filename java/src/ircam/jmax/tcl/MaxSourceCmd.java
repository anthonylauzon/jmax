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
import java.io.*;

import ircam.jmax.*;

/*
 * This class implements  a version of the built-in "source"  that do not
 * recursively halt the sourcing of file in case of an error, but just print
 * the error and return.
 * Also, this version provide a number of predefined local variables loading the file
 * (currently, $dir, that point to the current directory)
 * It work together with the _sysSourceFile command and the _doSourceFile tcl function
 * from the intrinsics.tcl file.
 */

class MaxSourceCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[])  throws   TclException
  {
    if (argv.length == 2)
      {
	File file;
	String fileName;
	String dirName;

	String inputPath = argv[1].toString();
	
	if (inputPath.charAt(0) == '~') {
	  fileName = ((String) MaxApplication.getProperty("user.home"))+ inputPath.substring(1);
	}
	else fileName = inputPath;

	file = new File(fileName);

	if (file.isAbsolute())
	  dirName = file.getParent();
	else  if (file.getParent() == null)
	  dirName = MaxApplication.getProperty("user.dir");
	else
	  dirName = ( MaxApplication.getProperty("user.dir") + 
		      MaxApplication.getProperty("file.separator") +
		      file.getParent());

	if (dirName != null)
	  interp.eval("_doSourceFile " + dirName + " " + fileName);
	else
	  interp.eval("_doSourceFile {} " + fileName);
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "fileName");
  }
}




