package ircam.jmax.fts.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/**
 * This class define the TCL Command <b>integerVector</b>,
 * that create a integer vector suitable for table objects
 * and the like.<br>
 * This command is a part of the <i>.tpa</i> file format. <p>
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *  integerVector <i>size { [value]* }</i>
 * </code> <p>
 *
 * Where value are integers.
 * Notes that size must be bigger than the number of elements
 * in the list, but the list can be smaller; in this case,
 * 0's will be added (?).
 */



public class FtsIntegerVectorCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2) 
      {
	FtsIntegerVector vector;
	int size;
	TclObject args;
	int[] values;

	size = TclInteger.get(interp, argv[1]);
	args = argv[2];


	vector = new FtsIntegerVector();
	vector.setSize(size);
	
	values = vector.getValues();

	int availables = TclList.getLength(interp, args);

	for (int i = 0; i < availables; i++)
	  {
	    TclObject obj = TclList.index(interp, args, i);

	    values[i] = TclInteger.get(interp, obj);
	  }

	interp.setResult(ReflectObject.newInstance(interp, vector));
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "<size> <values>");
      }
  }
}


