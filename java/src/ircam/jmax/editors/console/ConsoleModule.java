package ircam.jmax.editors.console;

import ircam.jmax.editors.console.tcl.*;

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
