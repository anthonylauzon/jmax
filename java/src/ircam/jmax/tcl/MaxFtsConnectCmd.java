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
      MaxApplication.getApplication().FTSConnect(argv[1].toString(),
						 argv[2].toString(),
						 argv[3].toString(),
						 argv[4].toString(),
						 argv[5].toString());
    else if (argv.length == 5)
      MaxApplication.getApplication().FTSConnect(argv[1].toString(),
						 argv[2].toString(),
						 argv[3].toString(),
						 argv[4].toString(),
						 null);
    else
      throw new TclException(interp, "wrong # args: should be ftsconnect  <type> ftsdir ftsname socket server port");
  }
}

