package ircam.jmax.fts.tcl;


import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>declare</b>,
 * that create a FTS declaration.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *    declare <i>patcher description graphic_data</i>
 * </code>
 */


class FtsDeclareCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 3)
      {
	throw new EvalException("missing argument; usage: declare <patcher> <description> [<graphic>]");
      }

    FtsObject patcher;
    FtsObject object;
    String    description;
    FtsServer server;

    // Retrieve the fts server (should be got from a Tcl variable ??)

    server = MaxApplication.getFtsServer();

    // Retrieve the arguments
    // this call should be substituted by a registration service call

    patcher            = server.getObjectByFtsId(Integer.parseInt(ca.argv(1)));
    description        = ca.argv(2);


    if (ca.argc > 3)
      object = FtsObject.makeFtsObject(patcher, description,
				       new FtsGraphicDescription(ca.argv(3)), true);
    else
      object = FtsObject.makeFtsObject(patcher, description, null, true);

    return String.valueOf(object.getObjId());
  }
}


