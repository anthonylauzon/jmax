package ircam.jmax.fts.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>template</b>,
 * used to declare a <i>tcl</i> template for FTS. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     template <i>name [proc] </i>
 * </code> <p>
 *
 * @see FtsTemplateTable
 */



class FtsTemplateCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 2)
      {
	throw new EvalException("missing argument; usage: template <name> [<proc>]");
      }

    String name;
    String proc;

    // Retrieve the arguments

    name = ca.argv(1);

    if (ca.argc == 3)
      proc = ca.argv(2);
    else
      proc = name;

    FtsTemplateTable.add(name, proc);

    return "";
  }
}

