package ircam.jmax.mda;

/** The mda module; the init function is called at init time
 *  by jmax, and install module related things
 */

public class MdaModule
{
  static public void initModule()
  {

    // Install the local mda entities

    MaxDataHandler.installDataHandler( new MaxTclFileDataHandler());

    // The following is to review.

    MaxDataHandler.installDataHandler( new MaxTclStringDataHandler());

    // Install the tcl commands for Mda

    ircam.jmax.mda.tcl.TclMdaPackage.installPackage();
  }
}
