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
package ircam.jmax.script.scm;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.script.*;
import ircam.jmax.script.pkg.*;

import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import java.awt.*;

/**  SchemeInterpreter
 *
 *  Extends the jMax interpreter with some additionnal useful
 *  methods. Provides some basic functions for Scheme interpreters.
 */
public abstract class SchemeInterpreter implements Interpreter
{ 
    public void addScriptMenu(String type, Script script, String name, Object key)
    {
	int modifiers = 0;
	int keycode = 0;
	if (key instanceof KeyStroke) {
	    KeyStroke stroke = (KeyStroke) key;
	    modifiers = stroke.getModifiers();
	    keycode = stroke.getKeyCode();
	}
	if (type.equals("all")) {
	    MaxDocumentType.addGlobalScript(new ScriptAction(script, name, modifiers, keycode));
	} else {
	    MaxDocumentType doctype = Mda.getDocumentTypeByName(type);
	    if (doctype == null) {
		System.out.println("Document type " + type + " is not defined");
	    } else {
		doctype.addScript(new ScriptAction(script, name, modifiers, keycode));
	    }
	}
    }

    /** Creates a KeyStroke object, given a key (as string) and the
     *  values of the modifiers. The only keys currently accepted are
     *  [0-9] and [a-z]. */
    public KeyStroke getKeyStroke(String keyStr, boolean alt, boolean ctrl, boolean meta, boolean shift)
    {
	int modifiers = 0;
	if (ctrl) modifiers += ActionEvent.CTRL_MASK;
	if (alt) modifiers += ActionEvent.ALT_MASK;
	if (meta) modifiers += ActionEvent.META_MASK;
	if (shift) modifiers += ActionEvent.SHIFT_MASK;
	char c = Character.toUpperCase(keyStr.charAt(0));
	int key;
	if (Character.isDigit(c)) {
	    key = KeyEvent.VK_0 + c - '0';
	} else if (('A' <= c) && (c <= 'Z')) {
	    key = KeyEvent.VK_A + c - 'A';
	} else {
	    return null;
	}
	return KeyStroke.getKeyStroke(key, modifiers);
    }

    public Object ask(String question, String type) {
	String s = ircam.jmax.dialogs.InputDialog.ask(question);
	return convertToScheme(s, type);
    }

    public abstract Object convertToScheme(String value, String type);

    /** Loads a file. */
    public abstract Object load(String path) throws ScriptException;

    /** Loads a file. */
    public abstract Object load(File file) throws ScriptException;

    /** Loads a file. This method does not throw an exception but
     *  displays an error message on the current output. */
    public Object loadSilently(String filename) {
	try {
	    File file = new File(filename);
	    String dir = file.getParent();
	    Object old_dir = lookup("dir");
	    // define("dir", dir);
	    Object result = load(file);
	    // define("dir", old_dir);
	    return result;
	} catch (Exception e) {
	    System.out.println("Couldn't load " + e.getMessage());
	    return null; 
	}
    }

    public Package loadPackage(Package pkg, File initfile) throws ScriptException
    {
	Object old_dir = lookup("dir");
	Object old_pkg = lookup("this-package");
	define("dir", initfile.getParent());
	define("this-package", pkg);
	load(initfile);
	define("dir", (old_dir == null) ? "" : old_dir);
	define("this-package", (old_pkg == null) ? "" : old_pkg);
	return pkg;
    }

    /**
     *  Unfortunately, Silk and Kawa don't use Java strings to
     *  represent Scheme strings. This method helps the conversion.
     *  The equivalent Scheme functions is "to-java-string" (defined
     *  in scheme_interface.scm). Note that when a Java method or
     *  constructor takes a Java string as argument, both Silk
     *  (SilkInterpreter) and Kawa will do the conversion. 
     */
    public static Object toJavaString(Object s) {
	if (s instanceof char[]) {  /* SILK */
	    return new String((char[]) s);
	} else { /* Kawa */
	    return s.toString();
	}
    }

    /**
     *  The Java "null" value is not represented in Scheme. Silk and
     *	Kawa handle this value differently. This method is provided to
     *	test in Scheme whether an object is null or not. 
     */
    public static boolean isNull(Object obj) 
    {
	return obj == null;
    }

    /**
     *  This methods creates a MaxSchemeDocument. It can be invoked in
     *  a Scheme file using (jmax <type> <version> <name> <info>
     *  <body>). Unfortunately the body has to be a string and not a
     *  procedure, because silk is not open enough to call a procedure
     *  with the current environment and I/O. Hopefully this will
     *  change in the future.
     */
    public MaxDocument loadMaxDocument(String type, String version, String name, 
				       String info, String body) throws ScriptException 
    {
	MaxDocument document = null;

	// Create a new instance of the type
	document = Mda.getDocumentTypeByName(type).newDocument(MaxApplication.getFts());

	if (false /*document instanceof MaxSchemeDocument FIXME*/) {
	    // Set the name and info
	    document.setName(name);
	    
	    // Eval the body inside the document 
	    // FIXME ((MaxSchemeDocument) document).eval(this, body);
	    
	    // Finally, return the document to the interpreter    
	    return document;
	} else {
	    throw new ScriptException(type + " is not a Scheme based jMax document");
	}
    }
}




