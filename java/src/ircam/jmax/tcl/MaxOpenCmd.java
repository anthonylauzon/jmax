/*
 * MaxOpenCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



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
    try
      {
	if (argv.length == 2)
	  {
	    MaxDocumentSource source = MaxDocumentSource.makeDocumentSource(argv[1].toString());

	    if (source != null)
	      {
		try
		  {
		    MaxDocument document;

		    document = Mda.loadDocument(source);
		    document.edit();
		  }
		catch (MaxDocumentException e)
		  {
		    throw new TclException(interp, e.toString());
		  }
	      }
	  }
	else
	  {	
	    throw new TclNumArgsException(interp, 1, argv, "<filename>");
	  }
      }
    catch (java.net.MalformedURLException e)
      {
	throw new TclException(interp, "Malformed URL " + argv[1].toString());
      }
  }
}



