package ircam.jmax.editors.ermes.tcl;


import tcl.lang.*;

import java.io.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;	//?
import ircam.jmax.editors.ermes.*;
import ircam.jmax.utils.*;

/**
 * The "elements" TCL command in ERMES. 
 * Returns the list of all the objects in the sketch.
 */

class GlobalProbeCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2) {
      if (argv[1].toString().equals("start")) {
	GlobalProbe.makeGlobalProbe();
	GlobalProbe.start();
      }
      else {
	GlobalProbe.stop();
	GlobalProbe.report();
      }
    }
  }
}



