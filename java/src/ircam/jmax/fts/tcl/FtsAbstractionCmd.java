package ircam.jmax.fts.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>abstraction</b>,
 * used to declare a <i>.pat</i> abstraction. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     abstraction <i>name path </i>
 * </code> <p>
 *
 * @see FtsAbstractionTable
 */

class FtsAbstractionCmd implements Command
{
  /** Method implementing the TCL command. */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 3)
      {
	throw new EvalException("missing argument; usage: abstraction <name> <path>");
      }

    String name;
    String path;

    // Retrieve the arguments

    name = ca.argv(1);
    path = ca.argv(2);

    FtsAbstractionTable.add(name, path);

    return "";
  }
}

