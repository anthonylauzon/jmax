//
// jMax
// Copyright (C) 1999 by IRCAM
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 
package ircam.jmax.tcl;


import tcl.lang.*;

import java.io.*;
import java.util.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.ermes.*;

class MaxOscilloscopeCmd implements Command
{
  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {

    if (argv.length == 1) {
      new Oscilloscope();
    }
    else if (argv.length ==3 ) {
      ErmesObject aObject = (ErmesObject) ReflectObject.get(interp, argv[1]);
      if (!(aObject instanceof OscillSource)) System.err.println("cannot connect to oscilloscope");
      else new Oscilloscope((OscillSource)aObject,TclInteger.get(interp, argv[2]) );
    }
    else throw new TclNumArgsException(interp, 1, argv, "");
  }
}



