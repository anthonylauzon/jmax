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
package ircam.jmax.editors.console;

/** The console module; the initModule function is called at init time
 *  by jmax, and install module related things
 */

public class ConsoleModule
{
  static public void initModule()
  {
    // Install the module tcl commands.
    ircam.jmax.editors.console.tcl.TclConsolePackage.installPackage();
  }
}


