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
package ircam.jmax.mda;

import tcl.lang.*;
import java.io.*;

/** This interface define the minimal requirements on 
 * a MaxDocument instance in order to save and store this instance
 * in TCL format; this also a way to check if an instance support
 * the Tcl format.
 */


public interface MaxTclDocument extends MaxTclInterpreter
{
  /** Ask the content to save itself as tcl code to
   * the given printwriter
   */

  abstract public void saveContentAsTcl(PrintWriter pw);
}










