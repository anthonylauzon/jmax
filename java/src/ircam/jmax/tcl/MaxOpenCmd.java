//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

// ^^^^ As all the other tcl commands, not work with multiserver.


package ircam.jmax.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.mda.*;


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



