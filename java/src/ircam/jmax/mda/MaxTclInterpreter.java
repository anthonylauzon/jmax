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
