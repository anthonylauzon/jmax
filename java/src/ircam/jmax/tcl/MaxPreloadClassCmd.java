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
package ircam.jmax.tcl;

import tcl.lang.*;

import java.lang.*;
import java.io.*;
import java.util.*;

import ircam.jmax.utils.*;


/**
 * A generic "preload class" command.
 */

class MaxPreloadClassCmd implements Command
{
  static MaxVector classVector = new MaxVector();
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

