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
import cornell.Jacl.*;

/**
 * The "FtsConnect" TCL command in ERMES.
 */

class MaxFtsConnectCmd implements Command
{
  /**

   * This procedure is invoked to establish an FTS connection

   */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {

    if (ca.argc < 2)
      {

	throw new EvalException("wrong # args: should be \"" + ca.argv(0)+"\" "+
				"none | local | ftsdir ftsname socket server port");

      }

    if (ca.argc == 2)
      {
	if (ca.argv(1).equals("none"))
	  throw new EvalException("\'none\' connections not implemented yet");
	else
	  throw new EvalException("wrong # args: should be \"" + ca.argv(0)+"\" "+
				  "none | local | ftsdir ftsname socket server port");
      }
	
    //remote connection: ftsdir, ftsname, connection_type, server, port

    if (ca.argc == 6)
      MaxApplication.getApplication().FTSConnect(ca.argv(1), ca.argv(2), ca.argv(3), ca.argv(4), ca.argv(5));
    else
      MaxApplication.getApplication().FTSConnect(ca.argv(1), ca.argv(2), ca.argv(3), ca.argv(4), null);

    return "";

  }

}

