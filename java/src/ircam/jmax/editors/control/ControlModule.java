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
package ircam.jmax.editors.control;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/** The console module; the initModule function is called at init time
 *  by jmax, and install module related things
 *
 * ^^^^ We have a nice problem here about the multi-server; how the tool finder
 * ^^^^ find about the server to use ? This should be solved in the UI framework
 * ^^^^ for multiserver jMax.
 */

public class ControlModule
{
  static public void initModule()
  {
    // Install the DspModule remote data

    Fts.registerRemoteDataClass("dspcontrol_data", ircam.jmax.fts.FtsDspControl.class);

    // Register the Control Panel as a tool

    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Control Panel";}
      public void open() { ControlPanelFrame.open(MaxApplication.getFts());}
    });
  }
}


