package ircam.jmax.fts.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>connect</b>,
 * that create a connection between two FTS objects.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     connect <i>from outlet to inlet</i>
 * </code> <p>
 *
 * @see FtsConnection
 */

class FtsConnectionCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 5)
      {
	throw new EvalException("missing argument; usage: connect <from> <outlet> <to> <inlet>");
      }

    FtsObject from;
    int       outlet;
    FtsObject to;
    int       inlet;
    FtsServer server;

    // Retrieve the fts server (should be got from a Tcl variable ??)

    server = MaxApplication.getFtsServer();

    // Retrieve the arguments
    // these calls should be substituted by a registration service call

    from   = server.getObjectByFtsId(ca.intArg(1));
    outlet = ca.intArg(2);
    to     = server.getObjectByFtsId(ca.intArg(3));
    inlet  = ca.intArg(4);

    new FtsConnection(from, outlet, to, inlet);

    return "";			// should return the connection ID in the registration service
  }
}

