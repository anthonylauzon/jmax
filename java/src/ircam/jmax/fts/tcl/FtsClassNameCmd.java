package ircam.jmax.fts.tcl;


import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>className</b>,
 * that get the class name of a FTS object. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *  className <i>obj</i>
 * </code> <p>
 *
 * @see FtsObject#getClassName
 */

class FtsClassNameCmd implements Command
{
  /** Method implementing the TCL command. */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 2)
      {
	throw new EvalException("missing argument; usage: className <obj>");
      }

    FtsServer server;
    FtsObject obj;

    // Retrieve the fts server (should be got from a Tcl variable ??)

    server = MaxApplication.getFtsServer();

    // Retrieve the arguments
    // these calls should be substituted by a registration service call

    obj   = server.getObjectByFtsId(ca.intArg(1));

    return obj.getClassName();
  }
}

