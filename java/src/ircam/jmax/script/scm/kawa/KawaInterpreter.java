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

package ircam.jmax.script.scm.kawa;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.script.*;
import ircam.jmax.script.pkg.*;
import ircam.jmax.script.scm.*;
import java.awt.event.*;
import java.io.*;
import kawa.lang.*;
import gnu.math.*;
import kawa.standard.Scheme;
import gnu.mapping.*;

/**
 *  KawaInterpreter
 *
 *  An implementation of the jMax interpreter using the Kawa
 *  interpreter (Scheme).  
 */
public class KawaInterpreter extends SchemeInterpreter 
{ 
    /** The Silk object ued for the evaluation. */
    Scheme itsInterp;

    /** Creates a new Silk interpreter */
    public KawaInterpreter()
    {
	super();
	itsInterp = new Scheme();
	Environment usrEnvironment = new Environment(itsInterp.getEnvironment());
	itsInterp.setEnvironment(usrEnvironment);
    }

    public void boot(String root) throws ScriptException 
    {
	try
	    {
		/* Define jmaxInterp so all Scheme file have acces to
                 * this object. The root property is also exported to
                 * Scheme for succesful living in the interpreter
                 * world. */
		define("jmax-interp", this);
		define("jmax-interp-name", "kawa");
		define("jmax-root", root);
		define("slash", File.separator);
		define("method", KawaJavaMethod.getProcedure());
		define("constructor", KawaJavaConstructor.getProcedure());

		/* Load the "jmaxboot.scm" file that will do whatever is needed to
		 * create the startup configuration, included reading user files
		 * installing editors, data types and data handlers. */
		load(root + System.getProperty("file.separator") + "scm" +
		     System.getProperty("file.separator") +  "jmaxboot.scm");
	    }
	catch (ScriptException e)
	    {
		throw new ScriptException("Scheme error in initialization: " + e.getMessage());
	    }	
    }

    /** The default input */
    Reader input;

    /** Sets input port. */
    public void setInput(Reader input) {
	this.input = input;
	InPort.setInDefault(new InPort(input));
    }

    /** Returns the input port. */
    public Reader getInput() 
    {
	return input;
    }

    /** The default output. */
    Writer output;

    /** Sets output print writer. */
    public void setOutput(Writer output) 
    {
	this.output = output;
	OutPort.setOutDefault(new OutPort(output));
    }

    /** Returns the output print writer. */
    public Writer getOutput() 
    {
	return output;
    }

    /** Sets the current environment. */
    public void setCurrentEnvironment(Environment env) 
    {
	itsInterp.setEnvironment(env);
    }

    /** Returns the current environment. */
    public Environment getCurrentEnvironment() 
    {
	return itsInterp.getEnvironment();
    }

    /** Defines a new variable in the current environment. Takes care
     *  to lower-case and internalize the variable name. */
    public Object define(String name, Object value) 
    {
	return getCurrentEnvironment().define(name.toLowerCase().intern(), value);
    }

    public Object lookup(String name) 
    {
	return getCurrentEnvironment().get(name.toLowerCase().intern());
    }

    public Object eval(Script script) throws ScriptException 
    {
	return script.eval();
    }

    public Object eval(String expr) throws ScriptException 
    {
	return itsInterp.eval(expr, getCurrentEnvironment());
    }

    public Object load(File file) throws ScriptException 
    {
	try  {
	    return load(new InPort(new FileReader(file), file.toString()));
	} catch (IOException e) {
	    throw new ScriptException(e.getMessage());
	}
    }

    public Object load(String path) throws ScriptException 
    {
	try  {
	    return load(new InPort(new FileReader(path), path));
	} catch (IOException e) {
	    throw new ScriptException(e.getMessage());
	}
    }

    public Object load(InPort in) throws ScriptException 
    {
	return itsInterp.eval(in, getCurrentEnvironment()); 
    }

    public Script convert(Object script) throws ScriptException
    {
	if (script instanceof String) {
	    return new DefaultSchemeScript(this, (String) script);
	} else if (script instanceof FString) {
	    return new DefaultSchemeScript(this, ((FString) script).toString());
	} else if (script instanceof Procedure) {
	    return new KawaScript(this, (Procedure) script);
	} else {
	    throw new ScriptException("Can't convert " + script.getClass().getName() + " to a script");
	}
    }

    /* FIXME */
    public boolean commandComplete(String s) 
    {
	return false;
    }

    public Project loadProject(Package context, File proj) throws ScriptException
    {
	throw new ScriptException("Not yet implemented");
    }

    public MaxDocument loadScriptedDocument(Package context, File script) throws ScriptException
    {
	throw new ScriptException("Not yet implemented");
    }

    public String getScriptLanguage()
    {
	return "scheme";
    }

    public static Object[] convertToJava(Object[] a, Class[] c) {
	if (a != null) {
	    int len = a.length;
	    for (int i = 0; i < len; i++) {
		a[i] = convertToJava(a[i], c[i]);
	    }
	}
	return a;
    }

    public static Object convertToJava(Object a, Class c) {
	if (a instanceof FString) {
	    return a.toString();
	} else if (a instanceof IntNum) {
	    if (c == Integer.TYPE) {
		return new Integer(((IntNum) a).intValue());
	    } else if (c == Long.TYPE) {
		return new Long(((IntNum) a).longValue());
	    } else {
		return a;
	    }
	} else if (a instanceof DFloNum) {
	    if (c == Float.TYPE) {
		return new Float(((DFloNum) a).floatValue());
	    } else if (c == Double.TYPE) {
		return new Double(((DFloNum) a).doubleValue());
	    } else {
		return a;
	    }
	} else if (a instanceof Char) {
	    return new Character(((Char) a).charValue());
	} else {
	    return a;
	} 
    }

    public Object convertToScheme(String value, String type) {
	try {
	    if (type.equals("int")) {
		return IntNum.make(Long.parseLong(value));
	    } else if (type.equals("float")) {
		return new DFloNum(new Double(value).doubleValue());
	    } else if (type.equals("string")) {
		return new FString(value);
	    } else if (type.equals("symbol")) {
		return value.intern();
	    } else if (type.equals("boolean")) {
		return (value.equals("true")) ? Boolean.TRUE : Boolean.FALSE;
	    } else {
		return new FString("Unknown type: " + type);
	    }
	} catch (Exception e) {
	    String m = (e.getMessage() != null) ? e.getMessage() : e.getClass().getName();
	    return new FString(m);
	}
    }

    public static Object convertToKawa(Object a) {
	if (a instanceof String) {
	    return new FString((String)a);
	} else if (a instanceof Integer) {
	    return IntNum.make(((Integer)a).longValue());
	} else if (a instanceof Long) {
	    return IntNum.make(((Long)a).longValue());
	} else if (a instanceof Float) {
	    return new DFloNum(((Float)a).doubleValue());
	} else if (a instanceof Double) {
	    return new DFloNum(((Double)a).doubleValue());
	} else if (a instanceof Character) {
	    return Char.make(((Character)a).charValue());
	} else if (a instanceof Boolean) {
	    return (((Boolean)a).booleanValue()) ? Boolean.TRUE : Boolean.FALSE;
	} else {
	    return a;
	}
    }

    public SettingsTable makeSettings(Package pkg) {
	return null;
    }

    public Object makeWorkSpace(Package pkg) { 
	return null;
    }

    public void setContext(Package pkg) {}
    public void removeContext(Package pkg) {}

    /** The main method starts a Scheme shell. Provided for testing purposes. */
    public static void main(String[] arg) throws ScriptException {
	KawaInterpreter kawasaki = new KawaInterpreter();
	kawasaki.define("jmax-interp", kawasaki);
	kawa.Shell.run(kawasaki.itsInterp);
    }
}


