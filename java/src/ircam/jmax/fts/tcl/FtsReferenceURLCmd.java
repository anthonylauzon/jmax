package ircam.jmax.fts.tcl;

import cornell.Jacl.*;
import java.io.*;
import java.util.*;

import ircam.jmax.fts.*;
/**
 * This class define the TCL Command <b>referenceURL</b>,
 * that register in the documentation data base an reference url
 * for a given FTS class.
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     referenceURL <i>className url </i>
 * </code>
 */

class FtsReferenceURLCmd implements Command
{
  /** Method implementing the TCL command */

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 3)
      {
	throw new EvalException("missing argument; usage: referenceURL <name> <url>");
      }

    String name;
    String url;

    // Retrieve the arguments

    name = ca.argv(1);
    url = ca.argv(2);

    FtsReferenceURLTable.add(name, url);

    return "";
  }
}

