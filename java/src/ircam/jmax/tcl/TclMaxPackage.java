/*
 * ErmesPackage.java --
 *
 */

package ircam.jmax.tcl;

import tcl.lang.*;
import java.lang.*;
import java.io.*;
import java.util.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 */


public class TclMaxPackage 
{
  static public void installPackage(Interp interp)
  {
    /* FTS */
    interp.createCommand("ftsconnect", new MaxFtsConnectCmd());

    /* misc */
    interp.createCommand("post", new MaxPostCmd());

    /* browser */
    interp.createCommand("open", new MaxOpenCmd());

    /* hooks */

    interp.createCommand("when", new MaxWhenCmd());
    interp.createCommand("runHooks", new MaxRunHooksCmd());

    interp.createCommand("systemProperty", new MaxSystemPropertyCmd());
    interp.createCommand("quit", new MaxQuitCmd());
  }
}







