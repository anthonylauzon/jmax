//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
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

/**
 * This interface define the minimal requirements on 
 * a MaxDocument instance in order to save and store this instance
 * in TCL format; this also a way to check if an instance support
 * the Tcl format.
 * Note that currently there are no Max documentes that are stored in tcl
 * format; this feature was introduced when the tcl textual format for patches
 * was supported, and dropped later (too slow).
 * In the transition to Scheme, it may be convinient to support scheme
 * based document (easier to handle declaratively than tcl, so a state or a configuration
 * can be easily saved as Scheme code); this is why this code and the other tcl
 * related classes exists in mda.
 */


public interface MaxTclDocument extends MaxTclInterpreter
{
  /** Ask the content to save itself as tcl code to
   * the given printwriter
   */

  abstract public void saveContentAsTcl(PrintWriter pw);
}










