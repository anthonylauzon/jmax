/** Class DefaultTclScript
 */
package ircam.jmax.script.tcl;
import ircam.jmax.script.*;

/** Implements a simple Tcl script consisting of a String expression. */

public class DefaultTclScript implements TclScript {
    String expr;
    Interpreter itsInterp;

    /** Creates a default Tcl script from a string. Eval will call the
     *  interpreter given as parameter.
     *
     *  @param interp The attached interpreter.
     *  @param e The string representing the Tcl expression.  */
    public DefaultTclScript(Interpreter interp, String s) 
    {
	itsInterp = interp;
	expr = s;
    }

    /** Eval this script using the script's attached interpreter. */
    public Object eval() throws ScriptException
    {
	if (itsInterp == null) {
	    throw new ScriptException("No interpreter defined.");
	}
	return itsInterp.eval(expr);
    }

    /** Evaluate the script using the given arguments. If you only
     *  need to pass 1, 2, 3, or 4 arguments you can use one of the
     *  other eval methods. */
    public Object eval(Object[] arg) throws ScriptException
    {
	if ((arg != null) && (arg.length > 0)) {
	    throw new ScriptException("Wrong number of arguments: " + arg.length + " instead of 0");
	}
	return eval();
    }

    /** Evaluate the script with one aruments. */
    public Object eval(Object a1) throws ScriptException
    {
	throw new ScriptException("Wrong number of arguments: 1 instead of 0");
    }

    /** Evaluate the script with two aruments. */
    public Object eval(Object a1, Object a2) throws ScriptException
    {
	throw new ScriptException("Wrong number of arguments: 2 instead of 0");
    }

    /** Evaluate the script with three aruments. */
    public Object eval(Object a1, Object a2, Object a3) throws ScriptException
    {
	throw new ScriptException("Wrong number of arguments: 3 instead of 0");
    }

    /** Evaluate the script with four aruments. */
    public Object eval(Object a1, Object a2, Object a3, Object a4) throws ScriptException
    {
	throw new ScriptException("Wrong number of arguments: 4 instead of 0");
    }

    /** Eval a string using the Tcl interpreter. For backward
     *  compatibility only. Use eval(ircam.jmax.script.Interpreter)
     *  instead. */
    public void eval(tcl.lang.Interp interp, String script) throws tcl.lang.TclException 
    {
	interp.eval(script);	
    }

    /** Eval a Tcl object using the Tcl interpreter. For backward
     *  compatibility only. Use eval(ircam.jmax.script.Interpreter)
     *  instead. */
    public void eval(tcl.lang.Interp interp, tcl.lang.TclObject script) throws tcl.lang.TclException 
    {
	//interp.eval(script);	
	new tcl.lang.TclException(interp, "How do I eval Tcl objects?");
    }
}

