package ircam.jmax.editors.control;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/** The console module; the initModule function is called at init time
 *  by jmax, and install module related things
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
      public void open() { ControlPanelFrame.open();}
    });
  }
}


