package ircam.jmax.fts.tcl;

import cornell.Jacl.*;
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

  public Object CmdProc(Interp interp, CmdArgs ca)
  {
    if (ca.argc < 4)
      {
	throw new EvalException("missing argument; usage: setProperty  <obj> <prop> <value>");
      }

    FtsServer server;
    FtsObject object;
    String    prop;
    String    sValue;
    Object    value;

    // Retrieve the fts server (should be got from a Tcl variable ??)

    server = MaxApplication.getFtsServer();

    // Retrieve the arguments
    // this call should be substituted by a registration service call

    object = server.getObjectByFtsId(ca.intArg(1));
    prop   = ca.argv(2);
    sValue = ca.argv(3);

    try
      {
	value= new Integer (Integer.parseInt(sValue));
      }
    catch (NumberFormatException e)
      {
	try
	  {
	    value = Float.valueOf(sValue);
	  }
	catch (NumberFormatException e2)
	  {
	    value = sValue;
	  }
      }

    object.putProperty(prop, value);

    return value;
  }
}

