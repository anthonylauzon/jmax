/*
 * MaxFtsConnectCmd.java
 *
 * Copyright (c) 1997 IRCAM.
 *
 */



package ircam.jmax.tcl;


import java.io.*;
import java.util.*;
import ircam.jmax.*;
import tcl.lang.*;

/**
 * The "FtsConnect" TCL command in ERMES.
 */

class MaxFtsConnectCmd implements Command
{
  /**

   * This procedure is invoked to establish an FTS connection

   */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 6)
      MaxApplication.getApplication().FTSConnect(new String(argv[1].toString()),
						 new String(argv[2].toString()),
						 new String(argv[3].toString()),
						 new String(argv[4].toString()),
						 new String(argv[5].toString()));
    else if (argv.length == 5)
      MaxApplication.getApplication().FTSConnect(new String(argv[1].toString()),
						 new String(argv[2].toString()),
						 new String(argv[3].toString()),
						 new String(argv[4].toString()),
						 null);
    else
      throw new TclNumArgsException(interp, 1, argv, "<type> <ftsdir> <ftsname> <socket> <server> <port>");
  }
}

