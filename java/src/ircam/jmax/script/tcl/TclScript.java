/** Class TclScript
 */
package ircam.jmax.script.tcl;
import ircam.jmax.script.*;

/** Interface of a Tcl script. It defines two addtionnal eval methods
 *  that take a tcl.lang.Interp object as argument. This methods
 *  originally come from the MaxTclInterpreter in the mda
 *  package. They're provided here for backward compatibility.  
 */

public interface TclScript extends Script {
    public void eval(tcl.lang.Interp interp, String  script) throws tcl.lang.TclException;
    public void eval(tcl.lang.Interp interp, tcl.lang.TclObject script) throws tcl.lang.TclException;
}

