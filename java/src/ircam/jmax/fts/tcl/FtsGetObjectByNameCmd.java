package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>getObjectByName</b>,
 * used to declare a <i>tcl</i> getObjectByName for FTS. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     getObjectByName <i>[parent] name</i>
 * </code> <p>
 *
 */


class FtsGetObjectByNameCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2)
      {
	String name;
	FtsObject obj;

	name = argv[1].toString();

	obj = FtsObject.getObject(name);

	if (obj != null)
	  interp.setResult(ReflectObject.newInstance(interp, obj));
	else
	  throw new TclException(interp, "Unknown object " + name);
      }
    else if (argv.length == 3)
      {
	String name;
	FtsContainerObject parent;
	FtsObject obj;

	name = argv[1].toString();
	parent = (FtsContainerObject) ReflectObject.get(interp, argv[2]);
	
	obj = parent.getObjectByName(name);

	if (obj != null)
	  interp.setResult(ReflectObject.newInstance(interp, obj));
	else
	  throw new TclException(interp, "Unknown object " + name);
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "[<parent>] <name>");
      }
  }
}

