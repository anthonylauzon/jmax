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



