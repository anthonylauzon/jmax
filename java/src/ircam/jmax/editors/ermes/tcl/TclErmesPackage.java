/*
 * ErmesPackage.java --
 *
 */

package ircam.jmax.editors.ermes.tcl;

import tcl.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 * Reduced: TCL is phasing out, kept only config related commands
 */


public class TclErmesPackage 
{
  /**
   * Default constructor. Create the built-in commands and load the
   * start-up scripts (ToDo).
   */

  static public void  installPackage()
  {
    Interp interp = MaxApplication.getTclInterp();

    /* preload commands*/

    interp.createCommand("toolbarPreloadIcon", new ErmesToolbarPreloadIconCmd());

    /* debug */

    interp.createCommand("probe", new GlobalProbeCmd());
  }
}







