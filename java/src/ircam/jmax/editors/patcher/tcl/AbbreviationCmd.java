package ircam.jmax.editors.patcher.tcl;



import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;

/**
 * The icon loader command.
 */

class AbbreviationCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	AddPopUp.addAbbreviation(argv[1].toString(), argv[2].toString());
      }
    else if (argv.length == 4)
      {
	AddPopUp.addAbbreviation(argv[1].toString(), argv[2].toString(), argv[3].toString());
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "abbreviation");
  }
}

