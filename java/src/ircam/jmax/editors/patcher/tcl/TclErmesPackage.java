//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.patcher.tcl;

import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.*;

/**
 * The Ermes package (as a set of TCL/ready commands).
 */


public class TclErmesPackage 
{
  static public void installPackage()
  {
    Interp interp = MaxApplication.getTclInterp();

    /* TCL generic */

    interp.createCommand("patcherMenu", new AbbreviationCmd());
  }
}







