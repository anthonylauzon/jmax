package ircam.jmax.fts.tcl;


import cornell.Jacl.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>object</b>,
 * that create an FTS object.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *      object <i>patcher description graphic_data</i>
 * </code>
 */

class FtsObjectCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 3)
      {
	throw new EvalException("missing argument; usage: object <patcher> <description> [<graphic>]");
      }

    FtsObject parent;
    FtsObject object;
    String    description;
    FtsServer server;

    // Retrieve the fts server (should be got from a Tcl variable ??)

    server = MaxApplication.getFtsServer();

    // Retrieve the arguments

    parent            = server.getObjectByFtsId(ca.intArg(1));
    description        = ca.argv(2);

    if (ca.argc > 3)
      object = FtsObject.makeFtsObject(parent, description,
				       new FtsGraphicDescription(ca.argv(3)), false);
    else
      object = FtsObject.makeFtsObject(parent, description);

    return String.valueOf(object.getObjId());
  }
}

