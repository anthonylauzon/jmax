package ircam.jmax.mda;

import tcl.lang.*;
import java.io.*;

/** This interface define the minimal requirements on 
 * a MaxData instance in order to save and store this instance
 * in TCL format; this also a way to check if an instance support
 * the Tcl format.
 */


public interface MaxTclData extends MaxTclInterpreter
{
  /** Ask the content to save itself as tcl code to
   * the given printwriter
   */

  abstract public void saveContentAsTcl(PrintWriter pw);
}










