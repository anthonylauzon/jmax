/*
 * ErmesPackage.java --
 *
 */

package ircam.jmax.editors.console.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 */


public class TclConsolePackage 
{
  /**
   * Default constructor. Create the built-in commands and load the
   * start-up scripts (ToDo).
   */

  static public void  installPackage()
  {
    Interp interp = MaxApplication.getTclInterp();

    /* open the console */

    interp.createCommand("openConsole", new ConsoleOpenCmd());
  }
}







