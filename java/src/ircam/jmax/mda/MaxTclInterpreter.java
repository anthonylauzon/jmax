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

/**
 * This interface define an object that can evaluate tcl code,
 * may be adding its context around
 */

public interface MaxTclInterpreter 
{
  abstract public void eval(Interp interp, String  script) throws tcl.lang.TclException;
  abstract public void eval(Interp interp, TclObject script) throws tcl.lang.TclException;
}
