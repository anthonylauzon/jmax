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
 *  atomList <i>{ [value]* }</i>
 * </code> 
 *
 */

class FtsAtomListCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    FtsAtomList atomList = new FtsAtomList();

    if (argv.length == 2)
      {
	Vector values = atomList.getValues();
	TclObject args;

	args = argv[1];	

	int availables = TclList.getLength(interp, args);

	for (int i = 0; i < availables; i++)
	  {
	    TclObject obj = TclList.index(interp, args, i);

	    try
	      {
		values.addElement(new Integer(TclInteger.get(interp, obj)));
	      }
	    catch (TclException e)
	      {
		try
		  {
		    values.addElement(new Float(TclDouble.get(interp, obj)));
		  }
		catch (TclException e2)
		  {
		    values.addElement(obj.toString());
		  }
	      }
	  }

	interp.setResult(ReflectObject.newInstance(interp, atomList));
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<arg_list>");
      }
  }
}




