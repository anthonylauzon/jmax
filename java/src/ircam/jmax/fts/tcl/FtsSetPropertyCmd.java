package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>setProperty</b>,
 * used to set a property of a FTS object. <br>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     setProperty  <i>obj property value</i>
 * </code>
 * 
 * @see FtsObject#putProperty
 */


class FtsSetPropertyCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 4)
      {
	FtsObject object;
	String    prop;
	Object    value;

	// Retrieve the arguments
	// this call should be substituted by a registration service call

	object = (FtsObject) ReflectObject.get(interp, argv[1]);
	prop   = new String(argv[2].toString());

	try
	  {
	    value = new Integer(TclInteger.get(interp, argv[3]));
	  }
	catch (TclException e)
	  {
	    try
	      {
		value = new Float(TclDouble.get(interp, argv[3]));
	      }
	    catch (TclException e2)
	      {
		value = new String(argv[3].toString());
	      }
	  }

	object.putProperty(prop, value);
      }
    else
      {
	throw new TclException(interp, "missing argument; usage: setProperty  <obj> <prop> <value>");
      }
  }
}

