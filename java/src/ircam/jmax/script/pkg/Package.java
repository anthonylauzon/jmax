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

/** Class Package
 * 
 * Dummy class, for future use.
 */
package ircam.jmax.script.pkg;
import ircam.jmax.script.*;
import java.util.*;
import java.io.*;

public class Package {
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

    /** Creates a new package with the given name and version. */
    public Package(PackageHandler handler, String name, String version) 
    {
	packageHandler = handler;
	this.name = name;
	this.version = version;
	initScript = name + ".scm";
	classLoader = null;
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

    /** Requires this package to have this or a higher version. Throws
     *  an exception if the package can't meet the requirements. */
    public void requireVersion(String version) throws Exception
    {
	return;
    }

    /** Loads the package. 
     * 
     * @param pkgDir The path to the root directory of the
     * package. See PackageHandler.locatePackage.
     * @param version The minimum version required for this package.
     * @param interp The interpreter in charge to load the init
     * file.*/
    public void load(File pkgDir, String version, Interpreter interp) throws Exception
    {
	interp.loadPackage(this, new File(pkgDir, initScript));
	requireVersion(version);
    }
    
    /** Load the class with the given name. The class is found along
     *  the local paths of this package. */
    public Class loadClass(String classname) throws Exception
    {
        if (classLoader == null) {
	    classLoader = new PackageClassLoader(this);
	}
	return classLoader.loadClass(classname, true);
    }

    /** Locates a file with a given name within the package. The file
     *  name should be it's full name, with extension and directories
     *  separated with the path separator.
     *  Ex. ircam/jmax/package/file.class. If you need to locate a
     *  file "Java style", use locateFile(file, extension). */
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

    /** Locates a file with the given extension. Dots '.' are
     *  considered file separators, and the extension is added to the
     *  file name. */
    public File locateFile(String filename, String extension) throws FileNotFoundException
    {
	filename = filename.replace('.', File.separatorChar) + extension;
	return locateFile(filename);
    }

    /** Appends a local path to the list of paths along which the
     *  package searches for files. */
    public void appendLocalPath(String path)
    {
	localPaths = (localPaths == null) ? path : localPaths + File.pathSeparator + path;
    }
}

