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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax; 

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.script.*;
import ircam.jmax.script.pkg.*;

/**
 * The main application class in jMax. Contains the global parameters 
 * and/or the global functionalities of the system. 
 */
public class MaxApplication
{
    /* (em 13-01-99)  in order to use the real system clipboard
     * instead of a local one, the following declaration would be sufficient: */
    public static Clipboard systemClipboard = Toolkit.getDefaultToolkit().getSystemClipboard();

    protected static ProjectHandler itsProjectHandler;
    protected static Project itsProject;

    public static Fts getFts() {
	return itsProject.getFts();
    }

    public static String getProperty(String key) {
	return itsInterpreter.getGlobalSettings().getSetting(key);
    }

    public static String setProperty(String key, String value) {
	String s = getProperty(key);
	itsInterpreter.getGlobalSettings().setSetting(key, value);
	return s;
    }

    public static Enumeration enumerateProperty(String key) {
	return itsInterpreter.getGlobalSettings().enumerateSetting(key);
    }
    
    /* The interpreter */
    protected static Interpreter itsInterpreter;

    public static Interpreter getInterpreter() {
	return itsInterpreter;
    }

    public static void setInterpreter(Interpreter i) {
	itsInterpreter = i;
    }

    protected static Interpreter makeInterpeter(String interpName) throws Exception {
	if (interpName.equalsIgnoreCase("jacl")) {
	    interpName = "ircam.jmax.script.tcl.TclInterpreter";
	} else if (interpName.equalsIgnoreCase("silk")) {
	    interpName = "ircam.jmax.script.scm.silk.SilkInterpreter";
	} else if (interpName.equalsIgnoreCase("kawa")) {
	    interpName = "ircam.jmax.script.scm.kawa.KawaInterpreter";
	}
	return (Interpreter) Class.forName(interpName).newInstance();
    }

    /** The package handler that keeps trace of the loaded package. */
     protected static MaxPackageLoader itsPackageLoader = null;

    public static MaxPackageLoader getPackageLoader() {
	return itsPackageLoader;
    }

    public static void setPackageLoader(MaxPackageLoader p) {
	itsPackageLoader = p;
    }

    /** The hook table */
    static MaxWhenHookTable itsHookTable = null;

    /** Functions to add application hooks */
    public static void addHook(String name, Script code) {
	if (itsHookTable == null) {
	    itsHookTable = new MaxWhenHookTable(); 
	}
	itsHookTable.addHook(name, code);
    }

    /** Functions to run application hooks */
    public static boolean runHooks(String name) {
	if (itsHookTable == null) {
	    return false;
	} else {
	    return itsHookTable.runHooks(name);
	}
    }

    public static void usage() {
	System.out.println("Usage: jmax [options] [project]");
	System.out.println("The options can be anything of the form:");
	System.out.println("  -name=value or -name:value");
	System.out.println("Please refer to the documentation.");
    }
    
    public static void main(String[] arg) {
	String interpName = "silk";
	String root = "/usr/lib/jmax";
	String projectName = null;
	File projectFile = null;

	try {
	    /* Parse the command line settings */
	    final MaxPackageData cmdLineSettings = new MaxPackageData();
	    StringBuffer buf = new StringBuffer();
	    for (int i = 0; i < arg.length; i++) {
		if (arg[i].charAt(0) != '-') {
		    if (i < arg.length - 1) {
			System.out.println("Project name should be last argument: " + arg[i]);
			usage();
			System.exit(0);
		    } else {
			projectName = arg[i];
			break;
		    }
		} else {
		    try {
			Object setting = SettingsParser.parseLine(arg[i].substring(1));
			cmdLineSettings.addElement(setting);
		    } catch (Exception e) {
			e.printStackTrace();
			System.out.println("Can't parse option " + arg[i]);
			usage();
			System.exit(0);
		    }
		}
	    }

	    /* Check whether the user specified an interpreter or a
	     * root. This is done the hard way since the settings are
	     * not hashed yet. */
	    int len = cmdLineSettings.size();
	    for (int i = 0; i < len; i++) {
		Assignment setting = (Assignment) cmdLineSettings.elementAt(i);
		if (setting.name.equals("interpreter")) {
		    interpName = setting.value;
		}
		if (setting.name.equals("root")) {
		    root = setting.value;
		}
	    }

	    /* Check whether the project file exists */
	    if (projectName == null) {
		projectName = root + File.separator + "packages" + File.separator + "jmax" + File.separator + "jmax.def";
	    }
	    projectFile = new File(projectName);
	    if (!projectFile.exists()) {
		System.out.println("Can't find project " + projectName);
		System.exit(0);
	    }

	    /* Create the interpreter, package handler and hook table */
	    itsPackageLoader = new MaxPackageLoader();
	    itsInterpreter = makeInterpeter(interpName);
	    itsHookTable = new MaxWhenHookTable(); 
	    itsProjectHandler = new ProjectHandler();
	    ircam.jmax.utils.Platform.setValues();

	    /** Globally defined settings.  */
	    itsInterpreter.define("pkg-path", null);
	    itsInterpreter.define("root", root);
	    itsInterpreter.define("slash", File.separator);


	    /* Create the package */
	    itsProject = new Project(itsPackageLoader, "<startup>"); 

	    /* This is a dirty hack. (Another one!). It assures that
	     * the jmax.scm file gets loaded before the jMax required
	     * packages are loaded. This is necessary since the
	     * package scripts may use the functions defined in
	     * jmax.scm */
	    // FIXME
	    {
		MaxPackage jmaxPkg;
		if (projectName.endsWith("jmax.def")) {
		    jmaxPkg = itsProject;
		} else {
		    itsPackageLoader.insert("jmax", "0.0.0");
		    jmaxPkg = itsPackageLoader.getPackage("jmax"); 
		}
		jmaxPkg.addListener(new DefaultPackageListener() {
		    public void finishedSettings(MaxPackage pkg, SettingsTable settings) {
			try {
			    String jmaxscript = settings.getSetting("dir") + File.separator + "scm" + File.separator + "intrinsics.scm";
			    System.out.println("Loading script " + jmaxscript);		
			    MaxApplication.getInterpreter().load(jmaxscript);
			} catch (Exception e) {
			    e.printStackTrace();
			}
		    }
		});
	    }

	    /* Make sure the command line settings get loaded at the
	     * end of the settings */
	    itsProject.addListener(new DefaultPackageListener() {
		public void finishedSettings(MaxPackage pkg, SettingsTable settings) {
		    MaxPackageDocument.hash(cmdLineSettings, settings);
		}
	    });

	    itsProjectHandler.openProjectFile(itsProject, projectFile);

	} catch (Exception e) {
	    String m = e.getClass().getName() + ((e.getMessage() != null) ? (": " + e.getMessage()) : "");
	    System.out.println("Can't boot the application: " + m);
	    e.printStackTrace(); // FIXME
	}
	
    }

    /**
     * Quit verify if there is anything to save
     * Currently, this is done in the wrong way: we get the list
     * of windows from the MaxWindowManager, and for those that 
     * are MaxEditor, we do the check;
     * In reality, we should look in the MDA document data base
     * to do the work.
     */
    public static void Quit() {
	ListModel windows;
	boolean someOneNeedSave = false;
	boolean doTheSave = false;

	// First, search if there is anything to save
	// Loop in all the documents in all the  types.
	ListModel types = Mda.getDocumentTypes();

    search: 
	for (int i = 0; i < types.getSize(); i++) {
	    MaxDocumentType type = (MaxDocumentType) types.getElementAt(i);
	    ListModel documents = type.getDocuments();
	  
	    for (int j = 0; j < documents.getSize(); j++) {
		MaxDocument document = (MaxDocument) documents.getElementAt(j);
	      
		if (! document.isSaved()) {
		    someOneNeedSave = true;
		    break search;
		}
	    }
	}
      
	// in such case, should give the offer to cancel the quit.
	if (someOneNeedSave) {
	    QuitDialog quitDialog = new QuitDialog(MaxWindowManager.getWindowManager().getTopFrame());
	
	    switch (quitDialog.getAnswer()) {
	    case QuitDialog.JUST_QUIT:
		doTheSave = false;
		break;
	    case QuitDialog.REVIEW_AND_QUIT:
		doTheSave = true;
		break;
	    case QuitDialog.CANCEL:
		return;
	    }
	
	    quitDialog.dispose();
	}

	// dispose (and optionally save) all the documents
	for (int i = 0; i < types.getSize(); i++) {
	    MaxDocumentType type = (MaxDocumentType) types.getElementAt(i);
	    ListModel documents = type.getDocuments();
	
	    for (int j = 0; j < documents.getSize(); j++) {
		MaxDocument document = (MaxDocument) documents.getElementAt(j);
	    
		if (doTheSave && (! document.isSaved())) {
		    if (YesOrNo.ask(MaxWindowManager.getWindowManager().getTopFrame(),
				    "Save " + document.getName(), "Save", "Don't Save")) {
			if (! document.canSave()) {
			    File file;
			    file= MaxFileChooser.chooseFileToSave(null, document.getDocumentFile(), "Save As");

			    if (file != null) {
				document.bindToDocumentFile(file);
			    }
			}
			if (document.canSave()) {
			    try {
				document.save();
			    } catch (MaxDocumentException e) {
				System.err.println(e.toString());
			    }
			} else {
			    new ErrorDialog(MaxWindowManager.getWindowManager().getTopFrame(), "Cannot Save " + document.getName());
			}
		    }		    
		    document.dispose();
		}
	    }
	}

	// Now, the quit is sure, we execute the exit hooks
	runHooks("exit");

	if (MaxWindowManager.getTopFrame() != null) {
	    MaxWindowManager.getTopFrame().setVisible(false);
	    MaxWindowManager.getTopFrame().dispose();
	}
	if (getFts() != null) {
	    getFts().stop();
	}
	Runtime.getRuntime().exit(0);
    }
}






