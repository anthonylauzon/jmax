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

package ircam.jmax.script.pkg;
import ircam.jmax.*;
import ircam.jmax.script.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;
import java.util.*;
import java.io.*;

/**
 *  Class MaxPackage
 * 
 * Dummy class, for future use.
 */
public class MaxPackage {
    /** The label of the package. The label is used the
     *  required-package and provided-package setting. It is also used
     *  by the package handler. */
    protected String label;

    /** The name of the package. The name is a user friendly name. */
    protected String name;

    /** The version of the package. */
    protected String version;

    /** Has the package been loaded. */
    protected boolean loaded = false;

    /** Has the package loaded its modules. */
    protected boolean loadedModules = false;

    /** The definition file of the package. */
    protected String itsDef;

    /** The directory of the package. */
    protected String itsDir;

    /** The document object representing the settingsfile */
    protected MaxPackageDocument itsDocument;

    /** The package handler that loaded this package. */
    protected MaxPackageLoader itsPackageLoader;

    /** The vector of packages this package depends on */
    protected Vector packages;

    /** The loader in charge of loading all the classes of this
     *  package. Every package has its class loader. */
    protected MaxPackageClassLoader itsClassLoader;

    /** The settings of the package */
    SettingsTable settings;

    /** Return the settingstable */
    public SettingsTable getSettings() 
    {
	return settings;
    }

    /** Return the value of a setting */
    public String getSetting(String key) 
    {
	return settings.getSetting(key);
    }

    /** Set the value of a setting */
    public void setSetting(String key, String value) 
    {
	settings.setSetting(key, value);
    }

    /** Return all values of a setting as an enumeration */
    public Enumeration enumerateSetting(String key) 
    {
	return settings.enumerateSetting(key);
    }

    /** The workspace of the package */
    Object workSpace;

    /** Give the loaded Java classes a change to know their package */
    public static MaxPackage loadingPackage = null;

    public Object getWorkSpace() 
    {
	return workSpace;
    }

    /** Listeners for changes of this packages */
    Vector listeners;

    public void addListener(MaxPackageListener listener) 
    {
	listeners.addElement(listener);
    }

    public void removeListener(MaxPackageListener listener) 
    {
	listeners.removeElement(listener);
    }

    public final static int LOADSETTINGS = 1;
    public final static int FINISHEDSETTINGS = 2;
    public final static int LOADPACKAGE = 3;
    public final static int FINISHEDPACKAGES = 4;
    public final static int LOADCLASS = 5;
    public final static int FINISHEDCLASS = 6;
    public final static int LOADSCRIPT = 7;
    public final static int FINISHEDSCRIPTS = 8;

    public void fireListeners(int action, Object arg) 
    {
	int len = listeners.size();
	for (int i = 0; i < len; i++) {
	    MaxPackageListener listener = (MaxPackageListener) listeners.elementAt(i);
	    switch (action) {
	    case LOADSETTINGS: listener.loadingSettings(this, (File) arg);
		break;
	    case FINISHEDSETTINGS: listener.finishedSettings(this, (SettingsTable) arg);
		break;
	    case LOADPACKAGE: listener.loadingPackage(this, (String) arg);
		break;
	    case FINISHEDPACKAGES: listener.finishedPackages(this);
		break;
	    case LOADCLASS: listener.loadingClass(this, (String) arg);
		break;
	    case FINISHEDCLASS: listener.finishedClasses(this);
		break;
	    case LOADSCRIPT: listener.loadingScript(this, (File) arg);
		break;
	    case FINISHEDSCRIPTS: listener.finishedScripts(this);
		break;
	    }
	}
    }

    /** Creates a new package with the given name and version. */
    public MaxPackage(MaxPackageLoader handler, String name, String version) 
    {
	itsPackageLoader = handler;
	label = name;
	this.name = name;
	this.version = version;
	itsDef = name + ".def";
	itsClassLoader = null;
	settings = MaxApplication.getInterpreter().makeSettings(this); 
	/* it's important to call makeWorkSpace after makeSettings */
	workSpace = MaxApplication.getInterpreter().makeWorkSpace(this);
	listeners = new Vector();
	packages = new Vector();


	//FIXME debugging
	addListener(new DefaultPackageListener(true));
    }

    /** Returns the name of the package. */
    public String getName() 
    {
	return name;
    }

    /** Returns a string representation of this package. */
    public String toString() 
    {
	return "MaxPackage[" + name + ';' + version + ';' + loaded + ']';
    }

    /** Returns the version of the package. */
    public String getVersion() 
    {
	return version;
    }

    /** Returns whether the package has been loaded. */
    public boolean isLoaded() 
    {
	return loaded;
    }

    /** Sets the package as loaded. */
    public void setLoaded(boolean l) 
    {
	loaded = l;
    }

    public String getDefinitionFile() 
    {
	return itsDef;
    }

    /**
     *  Requires this package to have this or a higher version. Throws
     *  an exception if the package can't meet the requirements. 
     */
    public void requireVersion(String version) throws Exception 
    {
	return;
    }

    /**
     * Loads the package. 
     * 
     * @param pkgDir The path to the root directory of the
     * package. See PackageLoader.locatePackage.
     * @param version The minimum version required for this package.
     * @param interp The interpreter in charge to load the init
     * file.
     */
    public synchronized void load(File pkgDir, String version) throws Exception 
    {
	File file = new File(pkgDir, itsDef);
	loadFromFile(file, version);
    }

    public synchronized void loadFromFile(File file, String version) throws Exception 
    {
	MaxPackage prevLoadingPackage = loadingPackage;
	loadingPackage = this;

	try {
	    itsDef = file.getName();
	    itsDir = file.getParent();

	    /* Start with clean settings */
	    defaultSettings();

	    /* Load the packages settings file */
	    fireListeners(LOADSETTINGS, file);
	    itsDocument = new MaxPackageDocument(file);
	    itsDocument.hash(settings);

	    /* Load the remaining settings files */
	    Enumeration settingFiles = settings.enumerateSetting("settings");
	    if (settingFiles != null) {
		while (settingFiles.hasMoreElements()) {
		    file = new File((String) settingFiles.nextElement());
		    if (file.exists()) {
			fireListeners(LOADSETTINGS, file);
			settings.loadSettings(file);
		    }
		}
	    }

 	    String s = settings.getSetting("name");
	    if (s.length() > 0) {
		name = s;
	    }
	    s = settings.getSetting("version");
	    if (s.length() > 0) {
		version = s;
	    }
	    s = settings.getSetting("provided-package");
	    if (s.length() > 0) {
	      itsPackageLoader.provide(s, version);
	    }

	    fireListeners(FINISHEDSETTINGS, settings);

	    /* check the version */
	    requireVersion(version);

	    /* Load the packages */
	    Enumeration packageNames = settings.enumerateSetting("required-package");
	    if (packageNames != null) {
		while (packageNames.hasMoreElements()) {
		    String pkgName = (String) packageNames.nextElement();
		    fireListeners(LOADPACKAGE, pkgName);
		    MaxPackage pkg = itsPackageLoader.require(pkgName, "0.0.0");
		    packages.addElement(pkg);
		}
	    }
	    fireListeners(FINISHEDPACKAGES, null);

	    /* Load the classes */
	    Enumeration claz = settings.enumerateSetting("classes");
	    if (claz != null) {
		while (claz.hasMoreElements()) {
		    String name = (String) claz.nextElement();
		    fireListeners(LOADCLASS, name);
		    loadClass(name);
		}
	    }
	    fireListeners(FINISHEDCLASS, null);

	    /* Load the scripts */
	    Enumeration scripts = settings.enumerateSetting("script");
	    if (scripts != null) {
		Interpreter interp = MaxApplication.getInterpreter();
		interp.setContext(this);
		interp.define("this-package", this);
		while (scripts.hasMoreElements()) {
		    file = new File((String) scripts.nextElement());
		    if (file.exists()) {
			fireListeners(LOADSCRIPT, file);
			interp.load(file);
		    }
		}
		interp.removeContext(this);
	    }
	    fireListeners(FINISHEDSCRIPTS, null);

	} catch (NoSuchElementException n) {
	    throw new Exception("The package name should be of the form <name>-<version>");
	} finally {
	    loadingPackage = prevLoadingPackage;
	}
    }

    /** Set the settings to their default values */
    public void defaultSettings() 
    {
	settings.setSetting("defition-file", itsDef);
	settings.setSetting("dir", itsDir);
	settings.setSetting("home", System.getProperty("user.home"));
	settings.setSetting("settings", null);
	settings.setSetting("provided-package", null);
	settings.setSetting("required-package", null);
	settings.setSetting("classpath", null);
	settings.setSetting("classes", null);
	settings.setSetting("script", null);
	settings.setSetting("debug", "false");
    }

    /**
     *  Load the class with the given name. The class is found along
     *  the local paths of this package. 
     */
    public Class loadClass(String classname) throws Exception
    {
        if (itsClassLoader == null) {
	    itsClassLoader = new MaxPackageClassLoader(this);
	}
	Class claz = itsClassLoader.loadClass(classname, true);
	return claz;
    }

    /** 
     *  Locates a class with the given name. 
     */
    public File locateClass(String classname) throws ClassNotFoundException
    {
	String filename = classname.replace('.', File.separatorChar) + ".class";
	Enumeration classpath = settings.enumerateSetting("classpath");
	if (classpath != null) {
	    while (classpath.hasMoreElements()) {
		File file = new File((String) classpath.nextElement(), filename);
		if (file.exists()) {
		    return file;
		}
	    }
	}
	throw new ClassNotFoundException(classname);
    }

    public void loadModules(Fts fts) throws Exception 
    {
	if (!loadedModules) {
	    /* Flag to avoid recursif calling */
	    loadedModules = true;
	    /* Load the modules of the required packages first */
	    int len = packages.size();
	    for (int i = 0; i < len; i++) {
		MaxPackage pkg = (MaxPackage) packages.elementAt(i);
		pkg.loadModules(fts);
	    }
	    String modulePath = settings.getSetting("module-path");
	    Enumeration modules = settings.enumerateSetting("module");
	    if (modules != null) {
		MaxVector arg = new MaxVector();
		arg.addElement("load");
		arg.addElement("module");
		arg.addElement("");
		arg.addElement("");
		while (modules.hasMoreElements()) {
		    String module = (String) modules.nextElement();
		    arg.setElementAt(module, 2);
		    arg.setElementAt(modulePath + File.separator + "lib" + module + ".so", 3);  // FIXME platform dependent
		    fts.ucsCommand(arg);
		}
 	    }
	}
    }
}





