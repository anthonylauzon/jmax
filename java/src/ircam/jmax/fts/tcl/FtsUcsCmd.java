package ircam.jmax.fts.tcl;

import ircam.jmax.*;
import tcl.lang.*;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * This class define the TCL Command <b>ucs</b>,
 * used to execute an FTS ucs command. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     ucs <i>args</i>
 * </code>
 */


class FtsUcsCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length >  1)
      {
	Vector args = new Vector();

	for (int i = 1; i < argv.length; i++)
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
		    args.addElement(new String(argv[i].toString()));
		  }
	      }
	  }

	Fts.getServer().ucsMessage(args);
	interp.resetResult();
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "[args]*");
      }
  }
}



