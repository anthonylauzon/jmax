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
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.mda.*;

// ^^^^ As all the other tcl commands, not work with multiserver.


/**

 * The generic "Open <filename>" TCL command.

 */

class MaxOpenCmd implements Command {

  /**
   * This procedure is invoked to execute a "Open" operation in Ermes
   * It calls the generic OpenFile procedure of the project manager
   * It is able to load everything that can be opened by the "open" menu
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	String inputPath = argv[1].toString();
	String path = new String();
	if (inputPath.charAt(0) == '~') {
	  path = ((String) MaxApplication.getProperty("user.home"))+ inputPath.substring(1);
	}
	File file = new File(path);

	try
	  {
	    MaxDocument document;
	    
	    document = Mda.loadDocument(MaxApplication.getFts(), file);
	    document.edit();
	  }
	catch (MaxDocumentException e)
	  {
	    throw new TclException(interp, e.toString());
	  }
      }
    else
      {	
	throw new TclNumArgsException(interp, 1, argv, "<filename>");
      }
  }
}



