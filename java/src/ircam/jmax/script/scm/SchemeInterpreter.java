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

/**  SchemeInterpreter
 *
 *  Extends the jMax interpreter with some additionnal useful
 *  methods. Provides some basic functions for Scheme interpreters.
 */

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

public abstract class SchemeInterpreter implements Interpreter, ActionListener
{ 
    /** The script menu item hash table will help to find the script
     *  bounded to a menu item. */
    protected Hashtable menuScriptTable;

    /** The script menu hash table will help to find the menu bounded
     *  to a document type. */
    protected Hashtable menuTypeTable;

    /** The package handler. */
    PackageHandler itsPackageHandler;

    public SchemeInterpreter() 
    {
	menuScriptTable = new Hashtable();
	menuTypeTable = new Hashtable();
	//JMenu allMenu = new JMenu("Script"); Wait till it swings
	Menu allMenu = new Menu("Script"); 
	menuTypeTable.put("all", allMenu);
	itsPackageHandler = new PackageHandler();
    }

    public void addScriptMenuItem(String type, String name, Object key, Script script)
    {
	// No Swing, just Java
//  	JMenu typeMenu = (JMenu) menuTypeTable.get(type);
//  	if (typeMenu == null) {
//  	    typeMenu = new JMenu(type);
//  	    menuTypeTable.put(type, typeMenu);
//  	}
//  	if (name.equals("-")) {
//  	    typeMenu.addSeparator();
//  	} else {
//  	    JMenuItem menuItem = new JMenuItem(name);
//  	    if (key instanceof KeyStroke) {
//  		menuItem.setAccelerator((KeyStroke) key);
//  	    }
//  	    menuItem.addActionListener(this);
//  	    menuScriptTable.put(menuItem, script);
//  	}

	Menu typeMenu = (Menu) menuTypeTable.get(type);
	if (typeMenu == null) {
	    /* This implementation does not check whether the document
	     *  type is registered at the Mda or not. It just adds the
	     *  menu in the appropriate scripting menu. The rationale
	     *  is that the user might define a script before the
	     *  document type is declared by the package. */
	    typeMenu = new Menu(type);
	    menuTypeTable.put(type, typeMenu);
	}
	if (name.equals("-")) {
	    typeMenu.addSeparator();
	} else {
	    MenuItem menuItem = new MenuItem(name);
	    menuItem.addActionListener(this);
	    /* I prefer storing the menu item in the table using the
	     * menu object as key instead of it's name to allow
	     * multiple use of the same menu name. */
	    menuScriptTable.put(menuItem, script);
	    typeMenu.add(menuItem);
	}
    }

    public void actionPerformed(ActionEvent e) 
    {
	Object src = e.getSource();
	//if (src instanceof JMenuItem) {
	if (src instanceof MenuItem) {
	    Script script = (Script) menuScriptTable.get(src);
	    //define();
	    try {
		script.eval();
	    } catch (ScriptException ex) {
		// What do we do? Print a message on the output?
		System.out.println("Dear user, please contact the author of the script.");
		System.out.println("An error slipped in: " + ex.getMessage());
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

    /** Returns the script menu for a given document type, or null if
     *  non is avaible. */
    public Menu getScriptMenu(String type)
    {
	return (Menu) menuTypeTable.get(type);	
    }

    /** Defines a new variable in the current environment. Takes care
     *  to lower-case and internalize the variable name. */
    public abstract Object define(String name, Object value);

    /** Loads a file. This method does not throw an exception but
     *  displays an error message on the current output. */
    public Object loadSilently(String path) { 
	return null; 
    }

    /** Unfortunately, Silk and Kawa don't use Java strings to
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

    /** The Java "null" value is not represented in Scheme. Silk and
     *	Kawa handle this value differently. This method is provided to
     *	test in Scheme whether an object is null or not. */
    public static boolean isNull(Object obj) 
    {
	return obj == null;
    }

    public PackageHandler getPackageHandler() 
    {
	return itsPackageHandler;
    }

    /** This methods creates a MaxSchemeDocument. It can be invoked in
     *  a Scheme file using (jmax <type> <version> <name> <info>
     *  <body>). Unfortunately the body has to be a string and not a
     *  procedure, because silk is not open enough to call a procedure
     *  with the current environment and I/O. Hopefully this will
     *  change in the future.*/
    public MaxDocument loadMaxDocument(String type, String version, String name, 
				       String info, String body) throws ScriptException 
    {
	MaxDocument document = null;

	// Create a new instance of the type
	document = Mda.getDocumentTypeByName(type).newDocument(MaxApplication.getFts());

	if (false /*document instanceof MaxSchemeDocument FIXME*/)
	    {
		// Set the name and info
		document.setName(name);
		
		// Eval the body inside the document 
		// FIXME ((MaxSchemeDocument) document).eval(this, body);
		
		// Finally, return the document to the tcl interpreter    
		return document;
	    }
	else
	    {
		throw new ScriptException(type + " is not a Scheme based jMax document");
	    }
    }
}
