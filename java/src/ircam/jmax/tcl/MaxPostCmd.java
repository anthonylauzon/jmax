/*
 * MaxPostCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */

package ircam.jmax.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
/**
 * The "post" TCL command in ERMES.
 */
class MaxPostCmd implements Command
{
  StringBuffer buffer = new StringBuffer(256);

  /**
   * This procedure is invoked to execute a post from the shell
   */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length >= 2)
      {
	buffer.setLength(0);

	//create the string to print

	for (int i = 1; i < argv.length; i++)
	  {
	    buffer.append(argv[i]);
	    buffer.append(" ");
	  }
    
	MaxApplication.getApplication().getPostStream().println(buffer.toString());
      }
    else
      {	
	throw new TclException(interp, "wrong number of arguments: usage: post args...");
      }
  }
}

