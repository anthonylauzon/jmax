/*
 * ErmesPackage.java --
 *
 */

package ircam.jmax.tcl;

import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 */


public class TclMaxPackage 
{
  static public void installPackage()
  {
    Interp interp = MaxApplication.getTclInterp();

    /* TCL generic */

    interp.createCommand("_sysSourceFile", new MaxSysSourceCmd());
    interp.createCommand("sourceFile", new MaxSourceCmd());

    /* FTS */
    interp.createCommand("ftsconnect", new MaxFtsConnectCmd());

    /* browser */
    interp.createCommand("open", new MaxOpenCmd());

    /* Splash screen support */

    interp.createCommand("splash", new MaxSplashCmd());
    
    /* Preload command */

    interp.createCommand("preloadClass", new MaxPreloadClassCmd());

    /* hooks */

    interp.createCommand("when", new MaxWhenCmd());
    interp.createCommand("runHooks", new MaxRunHooksCmd());

    interp.createCommand("systemProperty", new MaxSystemPropertyCmd());
    interp.createCommand("getMaxVersion", new MaxGetMaxVersionCmd());
    interp.createCommand("quit", new MaxQuitCmd());
  }
}







