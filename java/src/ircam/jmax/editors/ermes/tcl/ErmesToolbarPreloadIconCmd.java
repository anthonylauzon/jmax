package ircam.jmax.editors.ermes.tcl;



import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.*;//?????

/**
 * A "preload icon" command.
 */

class ErmesToolbarPreloadIconCmd implements Command
{

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
     if (argv.length == 2)
       ErmesSwToolbar.loadIcon(argv[1].toString());
     else
       throw new TclNumArgsException(interp, 1, argv, "iconFilePath");
  }
}
