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
import java.util.*;
import java.io.*;

/**
 *  Class Package
 * 
 * Dummy class, for future use.
 */
public class Package implements PackageListener {
    /** Should we print out debugging information. */
    protected boolean debug;

    /** The name of the package. */
    protected String name;

    /** The version of the package. */
    protected String version;

    /** Has the package been loaded. */
    protected boolean loaded = false;

    /** The script to init the package. */
    protected String initScript;

    /** The list of local paths along which the files of the package
     *  may be found. */
    // FIXME use properties? Could be handy. We'd benifit from
    // application wide properties.
    protected String localPaths = null;

    /** The path of this package. */
    String path = null;

    /** The package handler that loaded this package. */
    protected PackageHandler packageHandler;

    /** The loader in charge of loading all the classes of this
     *  package. Every package has its class loader. */
    protected PackageClassLoader classLoader;

    /** The settings of the package */
    SettingsTable settings;

    public SettingsTable getSettings() {
	return settings;
    }

    /** The workspace of the package */
    Object workSpace;

    public Object getWorkSpace() {
	return workSpace;
    }

    /** Listeners for changes of this packages */
    Vector listeners;

    public void addListener(PackageListener listener) {
	listeners.addElement(listener);
    }

    public void removeListener(PackageListener listener) {
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

    public void fireListeners(int action, Object arg) {
	int len = listeners.size();
	for (int i = 0; i < len; i++) {
	    PackageListener listener = (PackageListener) listeners.elementAt(i);
	    switch (action) {
	    case LOADSETTINGS: listener.loadingSettings((File) arg);
		break;
	    case FINISHEDSETTINGS: listener.finishedSettings((SettingsTable) arg);
		break;
	    case LOADPACKAGE: listener.loadingPackage((String) arg);
		break;
	    case FINISHEDPACKAGES: listener.finishedPackages();
		break;
	    case LOADCLASS: listener.loadingClass((String) arg);
		break;
	    case FINISHEDCLASS: listener.finishedClasses();
		break;
	    case LOADSCRIPT: listener.loadingScript((File) arg);
		break;
	    case FINISHEDSCRIPTS: listener.finishedScripts();
		break;
	    }
	}
    }

    /** Creates a new package with the given name and version. */
    public Package(PackageHandler handler, String name, String version) 
    {
	packageHandler = handler;
	this.name = name;
	this.version = version;
	initScript = name + ".scm"; // FIXME
	classLoader = null;
	settings = MaxApplication.getInterpreter().makeSettings(this); 
	/* it's important to call makeWorkSpace after makeSettings */
	workSpace = MaxApplication.getInterpreter().makeWorkSpace(this);
	listeners = new Vector();
    }

    /** Returns the name of the package. */
    public String getName() 
    {
	return name;
    }

    /** Returns a string representation of this package. */
    public String toString() 
    {
	return "Package[" + name + ';' + version + ';' + loaded + ']';
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
     * package. See PackageHandler.locatePackage.
     * @param version The minimum version required for this package.
     * @param interp The interpreter in charge to load the init
     * file.
     */
    public void load(File pkgDir, String version, Interpreter interp) throws Exception
    {
	interp.loadPackage(this, new File(pkgDir, initScript));
	requireVersion(version);
    }
    public void load(File pkgDir, String version) throws Exception
    {
	try {
	    /* Start with clean settings */
	    defaultSettings(pkgDir.getAbsolutePath());

	    /* Load the packages settings file */
	    File file = new File(pkgDir, name + ".def");
	    fireListeners(LOADSETTINGS, file);
	    settings.loadSettings(new File(pkgDir, name + ".def"));

 	    debug = settings.getSetting("debug").equals("true");
	    if (debug) {
		addListener(this);
	    }

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
	    fireListeners(FINISHEDSETTINGS, settings);

	    /* check the version */
	    requireVersion(version);

	    /* Load the packages */
	    Enumeration packages = settings.enumerateSetting("required-package");
	    if (packages != null) {
		while (packages.hasMoreElements()) {
		    String pkg = (String) packages.nextElement();
		    StringTokenizer st = new StringTokenizer(pkg, "-");
		    fireListeners(LOADPACKAGE, pkg);
		    packageHandler.require(st.nextToken(), st.nextToken());
		}
	    }
	    fireListeners(FINISHEDPACKAGES, null);

	    /* Load the classes */
	    Enumeration claz = settings.enumerateSetting("class");
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
	}
    }

    /** Set the settings to their default values */
    public void defaultSettings(String dir) {
	settings.setSetting("dir", dir);
	settings.setSetting("settings", null);
	settings.setSetting("provided-package", null);
	settings.setSetting("required-package", null);
	settings.setSetting("classpath", null);
	settings.setSetting("class", null);
	settings.setSetting("script", null);
	settings.setSetting("debug", "false");
    }

    /**
     *  Load the class with the given name. The class is found along
     *  the local paths of this package. 
     */
    public Class loadClass(String classname) throws Exception
    {
        if (classLoader == null) {
	    classLoader = new PackageClassLoader(this);
	}
	Class claz = classLoader.loadClass(classname, true);
	Class[] interf = claz.getInterfaces();
	for (int i = 0; i < interf.length; i++) {
	    if (interf[i].getName().equals("ircam.jmax.script.pkg.JavaExtension")) {
		JavaExtension extension = (JavaExtension) claz.newInstance();
		extension.init(MaxApplication.getInterpreter());
	    }
	}
	return claz;
    }

    /**
     *  Locates a file with a given name within the package. The file
     *  name should be it's full name, with extension and directories
     *  separated with the path separator.
     *  Ex. ircam/jmax/package/file.class. If you need to locate a
     *  file "Java style", use locateFile(file, extension). 
     */
    public File locateFile(String filename) throws FileNotFoundException
    {
	if (path == null) {
	    File filePath = packageHandler.locatePackage(name);
	    path = filePath.getAbsolutePath();
	}
	File file = new File(path, filename);
	if (file.exists()) {
	    return file;
	}
	if (localPaths != null) {
	    StringTokenizer st = new StringTokenizer(localPaths, File.pathSeparator);
	    while (st.hasMoreTokens()) {
		file = new File(path + File.separator + st.nextToken(), filename);
		if (file.exists()) {
		    return file;
		}
	    }
	}
	throw new FileNotFoundException(filename);
    }

    /** 
     *  Locates a file with the given extension. Dots '.' are
     *  considered file separators, and the extension is added to the
     *  file name. 
     */
    public File locateFile(String filename, String extension) throws FileNotFoundException
    {
	filename = filename.replace('.', File.separatorChar) + extension;
	return locateFile(filename);
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

    /**
     *  Appends a local path to the list of paths along which the
     *  package searches for files. 
     */
    public void appendLocalPath(String path)
    {
	localPaths = (localPaths == null) ? path : localPaths + File.pathSeparator + path;
    }

    public void loadingSettings(File f) {
	System.out.println("Loading settings file " + f);
    }

    public void finishedSettings(SettingsTable settings) {
	System.out.println("Finished loading settings");
    }

    public void loadingPackage(String p) {
	System.out.println("Loading package " + p);
    }

    public void finishedPackages() {
	System.out.println("Finished loading packages");
    }

    public void loadingScript(File f) {
	System.out.println("Loading script " + f);
    }

    public void finishedScripts() {
	System.out.println("Finished loading packages");
    }

    public void loadingClass(String name) {
	System.out.println("Loading class " + name);
    }

    public void finishedClasses() {
	System.out.println("Finished loading classes");
    }
}

