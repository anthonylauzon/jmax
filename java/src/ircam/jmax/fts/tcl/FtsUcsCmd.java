package ircam.jmax.fts.tcl;

import ircam.jmax.*;
import cornell.Jacl.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * This class define the TCL Command <b>ucs</b>,
 * used to execute an FTS ucs command. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     ucs <i>args</i>
 * </code>
 */


class FtsUcsCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (MaxApplication.getFtsServer() == null)
      throw new EvalException("Tcl error: ucs commands require an FTS connection");

    if (ca.argc < 2)
      {
	throw new EvalException("wrong # args: usage: ucs <arg_list>");
      }

    Vector args = new Vector();

    for (int i = 1; i < ca.argc; i++)
      {
	try
	  {
	    args.addElement(new Integer(ca.argv(i)));
	  }
	catch (NumberFormatException e)
	  {
	    try
	      {
		args.addElement(new Float(ca.argv(i)));
	      }
	    catch (NumberFormatException e1)
	      {
		// the arguments are always strings

		args.addElement(ca.argv(i));
	      }
	  }
      }

    MaxApplication.getFtsServer().ucsMessage(args);

    return "";
  }
}

