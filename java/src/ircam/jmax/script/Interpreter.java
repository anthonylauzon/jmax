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

/** Interface Interpreter
 *
 * An interface for the jMax interpreter. 
 */
package ircam.jmax.script;
import ircam.jmax.script.pkg.*;
import ircam.jmax.mda.*;
import java.awt.event.*;
import java.io.File;
import javax.swing.JMenu;
import java.awt.Menu;

public interface Interpreter 
{ 
    /** Defines a new variable in the current environment. */
    public Object define(String name, Object value);

    /** Look up a variable in the current environment. */
    public Object lookup(String name);

    /** This method allows the application to evaluate string
     *  commands. 
     *
     * @param expr The expression to be evaluated.
     */
    public Object eval(String expr) throws ScriptException;

    /** This method allows the application to evaluate interpreter
     * scripts. 
     *
     * @param script The script to be evaluated.
     */
    public Object eval(Script script) throws ScriptException;

    /** Load and evaluate a file.  
     *
     * @param file The file to be loaded.
     */
    public Object load(File file) throws ScriptException;

    /** Load and evaluate a file.  
     *
     * @param path The file to be loaded.
     */
    public Object load(String path) throws ScriptException;

    /** Convert a string to a Script interface. 
     * 
     *  @param script The script that needs conversion.  
     */
    public Script convert(Object script) throws ScriptException;

    /**
     * Get the language signature. Returns "scheme" for a Scheme
     * interpreter. 
     */
    public String getScriptLanguage();

    /**
     *  Add a menu item in the script menu. There's on general script
     *  menu, visible in all editors, and one script menu per document
     *  type. 
     *
     * @param name The label of the menu. 
     * @param key The key binding. Key should be of type KeyStroke to
     * be effective.
     * @param script The script to execute.  
     */
    public void addScriptMenu(String type, Script script, String name, Object key);

    /**
     *  Ask the user to enter a value in a dialog box. 
     *
     * @param question The question to ask the user.
     * @param type The return type of the answer. Valid types are
     * "boolean", "int", "float", "string" and "symbol". 
     */
    public Object ask(String question, String type);


    /** Testing */
    public SettingsTable makeSettings(MaxPackage pkg);
    public Object makeWorkSpace(MaxPackage pkg);
    public void setContext(MaxPackage pkg);
    public void removeContext(MaxPackage pkg);
    public SettingsTable getGlobalSettings();
}



