//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
// Based on Max/ISPW by Miller Puckette.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
//
package ircam.jmax.script.tcl;
import ircam.jmax.script.*;

/** Class DefaultTclScript
 *
 * Implements a simple Tcl script consisting of a String expression. 
 */
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

