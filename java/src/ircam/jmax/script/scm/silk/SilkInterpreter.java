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
// Authors: Peter Hanappe
// 

package ircam.jmax.script.scm.silk;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.script.*;
import ircam.jmax.script.scm.*;
import ircam.jmax.script.pkg.*;
import java.awt.event.*;
import java.io.*;
import silk.*;

/**  SilkInterpreter
 *
 *  An implementation of the jMax interpreter using the SILK package
 *  (Scheme).  
 */
public class SilkInterpreter extends SchemeInterpreter 
{ 
    /** The current environment used for the evaluation. */
    Environment currEnvironment;
    SilkSettingsEnvironment globalEnvironment;
    Environment schemeEnvironment;

    /** The Silk object ued for the evaluation. */
    Scheme itsInterp;

    public Scheme getScheme() {
	return itsInterp;
    } 

    /** The input port. */
    InputPort input;

    /** The output print writer. */
    PrintWriter output;

    class loadProcedure extends Procedure {
	public Object apply(Scheme interpreter, Object args) {
	    String name = "<unknown>";
	    try {
		name = new String((char[]) first(args)); 
		load(name);
		return Boolean.TRUE;
	    } catch (Exception e) {
		String m = (e.getMessage() != null) ? e.getMessage() : e.getClass().getName();
		System.out.println("Can't load file " + name + ": " + m);
		return Boolean.FALSE;
	    }
	}
    }

    /** Creates a new Silk interpreter */
    public SilkInterpreter() throws ScriptException
    {
	super();
	itsInterp = new Scheme(null);

	schemeEnvironment = new Environment();
	setCurrentEnvironment(schemeEnvironment);
	define("these-settings", schemeEnvironment);

	Primitive.installPrimitives(schemeEnvironment); 
	load(new InputPort(new StringReader(SchemePrimitives.CODE)));

	/* By default use the standard in/out */
	setInput(new InputPort(System.in));
	setOutput(new PrintWriter(System.out));

	/* We substitute Silk's "new", "method", and load primitives
	 * with our own. The "method" converts strings as
	 * expected. The constructor accepts an argument list. The
	 * function uses the current environment. */
	define("constructor", SilkJavaConstructor.getFactory());
	define("method", SilkJavaMethod.getFactory());
	define("invoke", new SilkJavaInvoke(SilkJavaInvoke.VIRTUAL));
	define("invoke-static", new SilkJavaInvoke(SilkJavaInvoke.STATIC));
	define("load", new loadProcedure());
	define("interpreter", this);

	globalEnvironment = new SilkSettingsEnvironment();
	globalEnvironment.parent = schemeEnvironment;
	setCurrentEnvironment(globalEnvironment);
    }

    /** Sets input port. */
    public void setInput(InputPort input) {
	this.input = input;
    }

    /** Returns the input port. */
    public InputPort getInput() {
	return input;
    }

    /** Sets output print writer. */
    public void setOutput(PrintWriter output) {
	this.output = output;
    }

    /** Returns the output print writer. */
    public PrintWriter getOutput() 
    {
	return output;
    }

    /** Sets the current environment. */
    public void setCurrentEnvironment(Environment env) 
    {
	currEnvironment = env;
    }

    /** Returns the current environment. */
    public Environment getCurrentEnvironment() 
    {
	return currEnvironment;
    }

    /** Defines a new variable in the current environment. Takes care
     *  to lower-case and internalize the variable name. */
    public Object define(String name, Object value) 
    {
	return currEnvironment.define(name.toLowerCase().intern(), value);
    }

    /** Returns the value of a variable in the current environment. */
    public Object lookup(String name) 
    {
	return currEnvironment.lookup(name.toLowerCase().intern());
    }

    public Object eval(Script script) throws ScriptException 
    {
	return script.eval();
    }

    public Object eval(String expr) throws ScriptException 
    {
	return load(new InputPort(new StringReader(expr)));
    }

    public Object load(File file) throws ScriptException 
    {
	try  {
	    return load(new InputPort(new FileReader(file)));	
	} catch (ScriptException sex) {
	    throw sex;
	} catch (Exception e) {
	    String m = (e.getMessage() != null) ? e.getMessage() : e.getClass().getName();	    
	    throw new ScriptException(m);
	}
    }

    public Object load(String path) throws ScriptException 
    {
	try  {
	    return load(new InputPort(new FileReader(path)));
	} catch (ScriptException sex) {
	    throw sex;
	} catch (Exception ex) {
	    String m = (ex.getMessage() != null) ? ex.getMessage() : ex.getClass().getName();	    
	    throw new ScriptException(m);
	}
    }

    /** This method implements what Silk should do: loading from an
     *  input stream using the current environment. */
    public Object load(InputPort in) throws ScriptException 
    {
	try {
	    Object x = null;
	    Object y = null;
	    while (true) {
		x = in.read();
		if (InputPort.isEOF(x)) {
		    return y;
		}
		y = itsInterp.eval(x, currEnvironment); 
	    }
	} catch (Exception ex) {
	    String m = (ex.getMessage() != null) ? ex.getClass().getName() + ": " + ex.getMessage() : ex.getClass().getName();	    
	    throw new ScriptException(m);
	}
    }

    public Script convert(Object script) throws ScriptException
    {
	if (script instanceof String) {
	    return new DefaultSchemeScript(this, (String) script);
	} else if (script instanceof char[]) {
	    return new DefaultSchemeScript(this, new String((char[]) script));
	} else if (script instanceof Procedure) {
	    return new SilkScript(this, (Procedure) script);
	} else {
	    throw new ScriptException("Invalid script.");
	}
    }

    public String getScriptLanguage()
    {
	return "scheme";
    }

    public Object convertToScheme(String value, String type) {
	try {
	    if (type.equals("int")) {
		return new Long(value);
	    } else if (type.equals("float")) {
		return new Double(value);
	    } else if (type.equals("string")) {
		char[] c = new char[value.length()];
		value.getChars(0, value.length(), c, 0);
		return c;
	    } else if (type.equals("symbol")) {
		return value.intern();
	    } else if (type.equals("boolean")) {
		return (value.equals("true")) ? Boolean.TRUE : Boolean.FALSE;
	    } else {
		return "Unknown type: " + type;
	    }
	} catch (Exception e) {
	    return  (e.getMessage() != null) ? e.getMessage() : e.getClass().getName();
	}
    }


  public SettingsTable getGlobalSettings() {
    return globalEnvironment;
  }

    public SettingsTable makeSettings(MaxPackage pkg) {
	SilkSettingsEnvironment env = new SilkSettingsEnvironment();
	env.parent = globalEnvironment;
	return env;
    }

    public Object makeWorkSpace(MaxPackage pkg) {
	Environment env = new Environment();
	env.parent = (Environment) pkg.getSettings();
	env.define("these-settings", pkg.getSettings());
	return env;
    }

    MaxPackage context;

    public void setContext(MaxPackage pkg) {
	context = pkg;
	setCurrentEnvironment((Environment) pkg.getWorkSpace());
    }

    public void removeContext(MaxPackage pkg) {
	if (context == pkg) {
	    setCurrentEnvironment(((Environment) pkg.getSettings()).parent);
	}
    }

    /** This method implements the readEvalWriteLoop from silk.Scheme,
     *  but uses the current environment and output stream */
    public void readEvalWriteLoop() {
	Object x;
	Object y;
	while (true) {
	    try {
		output.print("> "); 
		output.flush();
		x = input.read();
		if (InputPort.isEOF(x)) {
		    return;
		}
		y = itsInterp.eval(x, currEnvironment); 
		SchemeUtils.write(y, output, true); 
		output.println(); 
		output.flush();
	    } catch (Exception e) { 
		e.printStackTrace();
		output.println("**** ERROR: " + e.getMessage());
	    }
	}
    }

    /** The main method starts a read-eval-print loop using standard
     *  IO. Provided for testing purposes. */
    public static void main(String[] arg) throws ScriptException {
	new SilkInterpreter().readEvalWriteLoop();
    }
}




