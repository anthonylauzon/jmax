package ircam.jmax.editors.patcher.tcl;



import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;

/**
 * Define the commands:
 *
 * patcherMenu add <name> <description> <message>
 * patcherMenu add <submenus> <name> <description> <message>
 * patcherMenu addAndEdit <name> <description> <message>
 * patcherMenu addAndEdit <submenus> <name> <description> <message>
 */

class AbbreviationCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if ((argv.length < 5) || (argv.length > 6))
      throw new TclNumArgsException(interp, 1, argv, "patcherMenu");

    String subcmd = argv[1].toString();

    if (subcmd.equals("add") && argv.length == 5)
      {
	AddPopUp.addAbbreviation(argv[2].toString(), argv[3].toString(), argv[4].toString(), false);
      }
    else if (subcmd.equals("add") && argv.length == 6)
      {
	AddPopUp.addAbbreviation(argv[2].toString(), argv[3].toString(),
				 argv[4].toString(), argv[5].toString(), false);
      }
    else if (subcmd.equals("addAndEdit") && argv.length == 5)
      {
	AddPopUp.addAbbreviation(argv[2].toString(), argv[3].toString(), argv[4].toString(), true);
      }
    else if (subcmd.equals("addAndEdit") && argv.length == 6)
      {
	AddPopUp.addAbbreviation(argv[2].toString(), argv[3].toString(), 
				 argv[4].toString(), argv[5].toString(), false);
      }
  }
}

