package ircam.jmax.tcl;



import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

/**
 * A generic "preload class" command.
 */

class MaxPreloadClassCmd implements Command
{
  static Vector classVector = new Vector();
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 2) {
       try { 
	 classVector.addElement(Class.forName(argv[1].toString()));
       }     
       catch(ClassNotFoundException e) {System.err.println("preloading error: "+e.toString());}
    }
    else
      throw new TclNumArgsException(interp, 1, argv, "iconFilePath");
  }
}
