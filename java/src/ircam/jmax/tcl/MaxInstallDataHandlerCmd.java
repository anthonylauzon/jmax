/*
 * MaxInstallDataHandlerCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */


package ircam.jmax.tcl;


import java.io.*;
import java.util.*;
import tcl.lang.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/**
 * The "systemProperty" TCL command.
 */

class MaxInstallDataHandlerCmd implements Command
{
  /**
   * This procedure is invoked to install new data handler for a given data type
   */
  
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	MaxDataHandler aDataHandler;
	try {
	  aDataHandler = (MaxDataHandler) Class.forName(argv[1].toString()).newInstance();
	} catch (Exception e) {
	  System.out.println("error installing Data handler " +argv[1].toString());
	  throw new TclException(1);
	}
	MaxDataHandler.installDataHandler(aDataHandler);
	//everything is OK... proceed
	interp.setResult(ReflectObject.newInstance(interp, aDataHandler));
      }
    else
      throw new TclNumArgsException(interp, 2, argv, "data_type_name data_editor_name");
  }
}



