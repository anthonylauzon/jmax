package ircam.jmax.fts.tcl;

import ircam.jmax.*;
import cornell.Jacl.*;

import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>init</b>,
 * that send to a patcher the after-load init message
 * (that run the  loadbang like operations).<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     init <i>obj</i>
 * </code>
 */


class FtsInitCmd implements Command
{
  
  /** Method implementing the TCL command */
  
  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 2)
      throw new EvalException("wrong # args: should be \"" + ca.argv(0)+" <id>");

    MaxApplication.getFtsServer().getObjectByFtsId(ca.intArg(1)).loaded();

    return "";
  }
}



