package ircam.jmax.fts.tcl;


import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>mess</b>,
 * that send an arbitrary FTS message to an FTS objects.<br>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     mess <i> obj inlet selector arg_list</i>
 * </code>
 *
 * @deprecated
 */

class FtsMessCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    String idarg;

    if (argv.length < 4)
      {
	throw new TclException(interp, "wrong number of args: usage: mess <obj> <inlet> <selector> <arg_list>");
      }

    Vector args = new Vector();
	
    for (int i = 4; i < argv.length; i++)
      {
	try
	  {
	    args.addElement(new Integer(TclInteger.get(interp, argv[i])));
	  }
	catch (TclException e)
	  {
	    try
	      {
		args.addElement(new Float(TclDouble.get(interp, argv[i])));
	      }
	    catch (TclException e2)
	      {
		args.addElement(argv[i].toString());
	      }
	  }
      }

    try
      {
	FtsObject fo = (FtsObject) ReflectObject.get(interp, argv[1]);
	
	fo.sendMessage(TclInteger.get(interp, argv[2]), argv[3].toString(), args);
      }
    catch (TclException e)
      {
	MaxApplication.getFtsServer().sendNamedObjectMessage(argv[1].toString(),
							  TclInteger.get(interp, argv[2]),
							  argv[3].toString(),
							  args);
      }
  }
}




