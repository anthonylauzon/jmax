//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.y
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

/** Class TclInterpreter
 *
 * An Tcl interpreter for jMax. 
 */
package ircam.jmax.script.tcl;
import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.script.*;
import ircam.jmax.script.pkg.*;
import java.awt.event.*;
import java.awt.Menu;
import java.io.File;
import tcl.lang.*;

public class TclInterpreter implements ircam.jmax.script.Interpreter 
{ 
    /** We delegate all Tcl call to the real Tcl interpreter. */
    private tcl.lang.Interp itsInterp = null;

    /** The result of the last evaluation */
    private Object itsResult = null;

    /** Return the lang.tcl.Interp object. This is only temporary! */
    public Interp getTclInterp()
    {
	return itsInterp;
    }

    /** Return the result of the last evaluation. */
    public Object getResult() 
    {
	return itsResult;
    }

    public void boot(String root) throws ScriptException {
	itsInterp = new tcl.lang.Interp(); 	

	// moved from MdaModule to here
	Mda.installDocumentHandler(new MaxTclExeDocumentHandler());
	Mda.installDocumentType(new MaxTclExeDocumentType());

	TclMaxPackage.installPackage(this);

	// Initialize all the submodules; first the kernel modules
	TclMdaPackage.installPackage(this);
	TclFtsPackage.installPackage(this);

	// then the builtin editors 
	TclConsolePackage.installPackage(this);
	TclErmesPackage.installPackage(this);

	// Before booting the server, check if it is asked to run in real-time mode,
	// and if yes, inform the application layer
	/* ??? */
	try
	    {
		// Load the "jmaxboot.tcl" file that will do whatever is needed to
		// create the startup configuration, included reading user files
		// installing editors, data types and data handlers
	
		itsInterp.evalFile(root + getSystemProperty("file.separator") + "tcl" +
		 		   getSystemProperty("file.separator") +  "jmaxboot.tcl");
	    }
	catch (tcl.lang.TclException e)
	    {
		throw new ScriptException("TCL error in initialization: " + itsInterp.getResult());
	    }
    }

    /** Creates a new TCL command */
    public void createCommand(String name, Command cmd) 
    {
	itsInterp.createCommand(name, cmd);
    }

    public String getSystemProperty(String key)
    {
	return MaxApplication.getProperty(key);
    }

    public String setSystemProperty(String key, String value)
    {
	return MaxApplication.setProperty(key, value);
    }

    public boolean commandComplete(String s) 
    {
	return itsInterp.commandComplete(s);
    }

    public Object eval(String expr) throws ScriptException
    {
	try {
	    itsInterp.eval(expr);
	    itsResult = itsInterp.getResult();
	    return itsResult;
	} catch (tcl.lang.TclException e) {
	    throw new ScriptException(e.getMessage());
	}
    }

    public Object eval(Script script) throws ScriptException 
    {
	return script.eval();
    }

    public Object load(File file) throws ScriptException 
    {
	return eval("sourceFile " + file.getPath());
    }

    public Object load(String path) throws ScriptException 
    {
	return eval("sourceFile " + path);	
    }

    public Script convert(Object script) throws ScriptException
    {
	if (script instanceof String) {
	    return new DefaultTclScript(this, (String) script);
	} else {
	    throw new ScriptException("Invalid script.");
	}
    }

    public PackageHandler getPackageHandler() 
    {
	return null;
    }

    public ircam.jmax.script.pkg.JMaxPackage loadPackage(ircam.jmax.script.pkg.JMaxPackage context, File jpk) 
	throws ScriptException 
    {
	throw new ScriptException("Not yet implemented");
    }

    public ircam.jmax.script.pkg.Project loadProject(ircam.jmax.script.pkg.JMaxPackage context, File proj) 
	throws ScriptException
    {
	throw new ScriptException("Not yet implemented");
    }

    public MaxDocument loadScriptedDocument(ircam.jmax.script.pkg.JMaxPackage context, File script) 
	throws ScriptException
    {
	throw new ScriptException("Not yet implemented");
    }

    public String getScriptLanguage() 
    {
	return "tcl";
    }

    public void addScriptMenu(String type, Script script, String name, Object key) 
    {}

    public Object ask(String question, String type) 
    {
	return "Not yet implemented";
    }

    public String getProjectExtension()
    {
	return ".env";
    }
}
