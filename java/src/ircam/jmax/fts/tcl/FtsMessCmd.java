package ircam.jmax.fts.tcl;


import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>mess</b>,
 * that send an arbitrary FTS message to an FTS objects.<br>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     mess <i> id inlet selector arg_list</i>
 * </code>
 *
 * @deprecated
 */

class FtsMessCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    String idarg;

    if (ca.argc < 4)
      {

	throw new EvalException("wrong # args: usage: mess <id> <inlet> <selector> <arg_list>");

      }

    FtsServer server;

    // Retrieve the fts server (should be got from a Tcl variable ??)

    server = MaxApplication.getFtsServer();

    Vector args = new Vector();
	
    for (int i = 4; i < ca.argc; i++)
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
	    catch (NumberFormatException e2)
	      {
		// the arguments are always strings

		args.addElement(ca.argv(i));
	      }
	  }
      }

    idarg = ca.argv(1);

    try
      {
	FtsObject fo = null;
	
	fo = server.getObjectByFtsId(Integer.parseInt(idarg));

	if (fo != null)
	  {
	    fo.sendMessage(ca.intArg(2), ca.argv(3), args);
	    return "";
	  }
	else
	  throw new EvalException("no objects with id " + ca.argv(1));	
      }
    catch (NumberFormatException e)
      {
	server.sendNamedObjectMessage(idarg, ca.intArg(2), ca.argv(3), args);
      }

    return "";
  }
}



