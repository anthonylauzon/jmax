/*
 * ErmesPackage.java --
 *
 */

package ircam.jmax.tcl;

import cornell.Jacl.*;
import java.lang.*;
import java.io.*;
import java.util.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 */


public class TclMaxPackage extends cornell.Jacl.Package 
{
  /**
   * Default constructor. Create the built-in commands and load the
   * start-up scripts (ToDo).
   */
  public TclMaxPackage(Interp interp) {
    super(interp);

    /* FTS */
    interp.CreateCommand("ftsconnect", new MaxFtsConnectCmd());

    /* misc */
    interp.CreateCommand("post", new MaxPostCmd());

    /* browser */
    interp.CreateCommand("open", new MaxOpenCmd());

    /* hooks */

    interp.CreateCommand("when", new MaxWhenCmd());
    interp.CreateCommand("runHooks", new MaxRunHooksCmd());

    interp.CreateCommand("systemProperty", new MaxSystemPropertyCmd());
    interp.CreateCommand("quit", new MaxQuitCmd());
  }
  
  
  /**
   * This method is called when the "exit" command is issued.
   * @return false iff the "System.exit()" method shouldn't be
   * called.
   */
  public boolean ExitNotify() {    
    return false;
  }
}







