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
// Author: Peter Hanappe
//
package ircam.jmax.script.scm.silk;
import ircam.jmax.script.*;
import silk.*;

/**
 * SilkScript
 *
 * A wrapper script for a Silk Scheme procedure. 
 */
public class SilkScript implements Script {
    Procedure itsProcedure;
    Interpreter itsInterp;

    /**
     *  Creates a Scheme script from a Silk procedure. Eval will call
     *  the interpreter given as parameter.
     *
     *  @param interp The attached interpreter.
     *  @param e The string representing the Tcl expression.  */
    public SilkScript(Interpreter interp, Procedure proc) 
    {
	itsInterp = interp;
	itsProcedure = proc;
    }

    /**
     *  Eval this script script using the interpreter passed as argument.
     *
     *  @param interp The script's interpreter. */
    public Object eval(Interpreter interp) throws ScriptException 
    {
	return itsProcedure.apply((Scheme) interp, null);
    }

    /** Eval this script using the script's attached interpreter. */
    public Object eval() throws ScriptException 
    {
	if (itsInterp == null) {
	    throw new ScriptException("No interpreter defined.");
	}
	return itsProcedure.apply(((SilkInterpreter) itsInterp).getScheme(), null);
    }

    /**
     *  Evaluate the script using the given arguments. If you only
     *  need to pass 1, 2, 3, or 4 arguments you can use one of the
     *  other eval methods. */
    public Object eval(Object[] arg) throws ScriptException
    {
	if (itsInterp == null) {
	    throw new ScriptException("No interpreter defined.");
	}
	try {
	    return itsProcedure.apply(((SilkInterpreter) itsInterp).getScheme(), SchemeUtils.vectorToList(arg));
	} catch (Exception e) {
	    throw new ScriptException(e.getMessage());
	}
    }

    /** Same as SchemeUtils.cons. Just shorter to type. */
    protected static Pair cons(Object a, Object b) {
	return SchemeUtils.cons(a, b);
    }

    /** Evaluate the script with one argument. */
    public Object eval(Object a1) throws ScriptException
    {
	if (itsInterp == null) {
	    throw new ScriptException("No interpreter defined.");
	}
	try {
	    return itsProcedure.apply(((SilkInterpreter) itsInterp).getScheme(), cons(a1, null));
	} catch (Exception e) {
	    throw new ScriptException(e.getMessage());
	}
    }

    /** Evaluate the script with two arguments. */
    public Object eval(Object a1, Object a2) throws ScriptException
    {
	if (itsInterp == null) {
	    throw new ScriptException("No interpreter defined.");
	}
	try {
	    return itsProcedure.apply(((SilkInterpreter) itsInterp).getScheme(), cons(a1, cons(a2, null)));
	} catch (Exception e) {
	    throw new ScriptException(e.getMessage());
	}
    }

    /** Evaluate the script with three arguments. */
    public Object eval(Object a1, Object a2, Object a3) throws ScriptException
    {
	if (itsInterp == null) {
	    throw new ScriptException("No interpreter defined.");
	}
	try {
	    return itsProcedure.apply(((SilkInterpreter) itsInterp).getScheme(), cons(a1, cons(a2, cons(a3, null))));
	} catch (Exception e) {
	    throw new ScriptException(e.getMessage());
	}
    }

    /** Evaluate the script with four arguments. */
    public Object eval(Object a1, Object a2, Object a3, Object a4) throws ScriptException
    {
	if (itsInterp == null) {
	    throw new ScriptException("No interpreter defined.");
	}
	try {
	    return itsProcedure.apply(((SilkInterpreter) itsInterp).getScheme(), cons(a1, cons(a2, cons(a3, cons(a4, null)))));
	} catch (Exception e) {
	    throw new ScriptException(e.getMessage());
	}
    }
}
