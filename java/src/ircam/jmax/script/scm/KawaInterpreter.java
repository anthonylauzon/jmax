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
// Authors: Peter Hanappe

/**  KawaInterpreter
 *
 *  An implementation of the jMax interpreter using the Kawa
 *  interpreter (Scheme).  */

package ircam.jmax.script.scm;
import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.script.*;
import ircam.jmax.script.pkg.*;
import java.awt.event.*;
import java.io.*;
import kawa.lang.*;
import kawa.standard.Scheme;
import gnu.mapping.*;

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

    public Object eval(Script script) throws ScriptException 
    {
	return script.eval(this);
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

    /** Loads a file. This method does not throw an exception but
     *  displays an error message on the current output. */
    public Object loadSilently(String path) 
    {
	try  {
	    return load(new InPort(new FileReader(path), path));
	} catch (Exception e) {
	    // FIXME
	    //output.println(e.getMessage());
	    //e.printStackTrace();
	    System.out.println("Couldn't load " + e.getMessage());
	    return null;
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
	    // FIXME
	    throw new ScriptException("Procedure script are coming up");
	} else {
	    throw new ScriptException("Can't convert " + script.getClass().getName() + " to a script");
	}
    }

    /* FIXME */
    public boolean commandComplete(String s) 
    {
	return false;
    }

    public Package loadPackage(Package pkg, File initfile) throws ScriptException
    {
	define("dir", initfile.getParent());
	define("this-package", pkg);
	load(initfile);
	return pkg;
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

    /** The main method starts a Scheme shell. Provided for testing purposes. */
    public static void main(String[] arg) throws ScriptException {
	KawaInterpreter kawasaki = new KawaInterpreter();
	kawasaki.define("jmax-interp", kawasaki);
	kawa.Shell.run(kawasaki.itsInterp);
    }
}
