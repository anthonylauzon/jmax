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
// Authors: Maurizio De Cecco, Francois Dechelle, Peter Hanappe, Enzo Maggi, 
//          Norbert Schnell.
// 

/** DefaultSchemeScript
 *
 */
package ircam.jmax.script.scm;
import ircam.jmax.script.*;

/** Implements a simple Scheme script consisting of a String
 *  expression. The evaluation expects no arguments. */

public class DefaultSchemeScript implements Script {
    String expr;
    Interpreter itsInterp;

    /** Creates a default Scheme script from a string. Eval will call
     *  the interpreter given as parameter.
     *
     *  @param interp The attached interpreter.
     *  @param e The string representing the Tcl expression.  */
    public DefaultSchemeScript(Interpreter interp, String s) 
    {
	itsInterp = interp;
	expr = s;
    }

    /** Eval this script script using the interpreter passed as argument.
     *
     *  @param interp The script's interpreter. */
    public Object eval(Interpreter interp) throws ScriptException 
    {
	return interp.eval(expr);
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
}
