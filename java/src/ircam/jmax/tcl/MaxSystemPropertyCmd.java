/*
 * MaxSystemPropertyCmd.java
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
      throw new TclNumArgsException(interp, 1, argv, "propery");
  }
}



