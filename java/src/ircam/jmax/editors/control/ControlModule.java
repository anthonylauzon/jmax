package ircam.jmax.editors.control;

import ircam.jmax.fts.*;

/** The console module; the initModule function is called at init time
 *  by jmax, and install module related things
 */

public class ControlModule
{
  static public void initModule()
  {
    // Install the DspModule remote data

    Fts.registerRemoteDataClass("dspcontrol_data", ircam.jmax.editors.control.DspControl.class);
  }
}


