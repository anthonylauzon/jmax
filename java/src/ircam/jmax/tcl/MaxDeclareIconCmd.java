package ircam.jmax.tcl;



import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.toolkit.*;

/**
 * The icon loader command.
 */

class MaxDeclareIconCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	Icons.loadIcon(argv[1].toString(), argv[2].toString());
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "declareIcon");
  }
}

