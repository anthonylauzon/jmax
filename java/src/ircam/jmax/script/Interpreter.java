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
    /** Load the application wide settings. The name of the primary
     * init file is interpreter depended. The root directory is passed
     * as argument. From this file all settings should be loaded.  */
    public void boot(String root) throws ScriptException;

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

    /** Returns true if the strings s contains a complete interpreter
     *  command that can be subsequently evaluated. This method is
     *  used by the console to determine whether the buffer should be
     *  evaluated after a return's been pressed or whether it has to
     *  wait for further input. 
     *
     *  @param s The current text in the console buffer.  
     */
    public boolean commandComplete(String s);

    /** Returns the package handler. */
    public PackageHandler getPackageHandler(); 

    /** Load the package using the given initialization file.
     * 
     * @param pkg The package to be loaded.
     * @param initfile The initialization file of the package. 
     */
    public Package loadPackage(Package pkg, File initfile) throws ScriptException; 

    /** Load the package using in project script proj. Same thing as
     * (Project)loadScriptedDocument(proj). 
     * 
     * @param context
     * @param proj
     */
    public Project loadProject(Package context, File proj) throws ScriptException; 

    /** Scripted documents are documents written in the script
     * language that produce a new document of any type when
     * evaluated. A scripted document should start with the language
     * dependend command characters followed by "jmax
     * <script-lang>". For example for Scheme: ";jmax scheme". All
     * further information, such as the version, is coded in the
     * scripting language.  
     * 
     * @param context
     * @param script
     */
    public MaxDocument loadScriptedDocument(Package context, File script) throws ScriptException;

    /** Get the language signature. Returns "scheme" for a Scheme
     * interpreter. */
    public String getScriptLanguage();

    /** Add a menu item in the script menu. There's on general script
     *  menu, visible in all editors, and one script menu per document
     *  type. 
     *
     * @param type The document type to which this script applies
     * menu. "all" signifies the all document types. In that case the
     * script will be visible in all editors.
     * @param name The label of the menu. If the name is equal to "-"
     * a menu separator is inserted.
     * @param key The key binding. Key should be of type KeyStroke to
     * be effective.
     * @param script The script to execute.  */
    public void addScriptMenuItem(String type, String name, Object key, Script script);

    /** Returns the script menu for a given document type, or null if
     *  non is avaible. */
    public Menu getScriptMenu(String type);
}
