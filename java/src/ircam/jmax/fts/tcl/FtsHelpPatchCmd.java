package ircam.jmax.fts.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>helpPatch</b>,
 * that register in the help data base an help patch
 * for a given FTS class.
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     helpPatch <i>className filename </i>
 * </code>
 */

class FtsHelpPatchCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 3)
      {
	throw new EvalException("missing argument; usage: helpPatch <name> <filename>");
      }

    String name;
    String patch;

    // Retrieve the arguments

    name  = ca.argv(1);
    patch = ca.argv(2);

    FtsHelpPatchTable.add(name, patch);

    return "";
  }
}

