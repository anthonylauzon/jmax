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
 *  MaxPackageLoader
 * 
 *  The package loader is in charge of locating and loading a
 *  package. It keeps track of what packages has been loaded.  
 */

public class MaxPackageLoader {
    /** All the packages that the handler knows about are stored in the
     *  packageTable */
    private Hashtable packageTable;

    /** This script is executed as a last resort after the handler
     *  failed to load a package. */
    private Script unknownScript;

    /** Set the unknown script. */
    public void setUnknownScript(Script script) 
    {
	unknownScript = script;
    }

    /** Returns the unknown script. */
    public Script getUnknownScript() 
    {
	return unknownScript;
    }

    /** Constructs a new MaxPackageLoader. */
    public MaxPackageLoader() 
    {
	packageTable = new Hashtable();
    }

    /** Indicate that the package has been loaded and initialized. If
     *  the package is not yet known to the handler, a new entry in
     *  the table is created. */
    public void provide(String packageName, String version) 
    {
	MaxPackage pkg = (MaxPackage) packageTable.get(packageName);
	if (pkg == null) {
	    pkg = new MaxPackage(this, packageName, version);
	    packageTable.put(packageName, pkg);
	}
	pkg.setLoaded(true);
    }

    /** Insert the package in the table is it isn't there already. The
     *  package is not marked as being loaded. */
    public void insert(String packageName, String version) 
    {
	MaxPackage pkg = (MaxPackage) packageTable.get(packageName);
	if (pkg == null) {
	    pkg = new MaxPackage(this, packageName, version);
	    packageTable.put(packageName, pkg);
	}
    }

    /** Removes all data concerning the package. This method does
     *  nothing if the package is not in the table. */
    public void forget(String packageName) 
    {
	packageTable.remove(packageName);
    }


    /** Makes sure that a package is loaded. If the package is not
     *  loaded or known the package's load script and/or the unknown
     *  script are used to load the package. This method does the same
     *  as require except that it doesn't throw an exception if the
     *  package is not found but prints out a message instead. */
    public void requireSilently(String packageName, String version)
    {
	try {
	    require(packageName, version);
	} catch (Exception e) {
	    System.out.println("Error loading package " + packageName + ": package not found");
	}
    }

    /** Makes sure that a package is loaded. If the package is not
     *  loaded or known the package's load script and/or the unknown
     *  script are used to load the package. */
    public MaxPackage require(String packageName, String version) throws Exception
    {
	MaxPackage pkg = (MaxPackage) packageTable.get(packageName);
	File pkgPath = null;
	try {
		    
	    if (pkg != null) {
		if (pkg.isLoaded()) {
		    pkg.requireVersion(version);
		    return pkg;
		} else {
		    
		    pkgPath = locatePackage(packageName);
		    
		    if (pkgPath != null) {
			pkg.load(pkgPath, version);
			return pkg;
		    } else if (unknownScript != null) {
			
			unknownScript.eval(packageName, version);
			if (pkg.isLoaded()) {
			    /* In case the unknown script loaded the package. */
			    pkg.requireVersion(version);
			    return pkg;
			} else {
			    /* In case the unknown script added a new package path. */
			    pkgPath = locatePackage(packageName);
			    if (pkgPath != null) {
				pkg.load(pkgPath, version);
				return pkg;
			    }
			}
		    }
		}
	    } else {
		/* Keep an entry of the package in the table. */ 
		insert(packageName, version);

		/* And start al over again. */
		return require(packageName, version);
	    }
	} catch (ScriptException e)  {
	    throw new Exception(e.getMessage());
	} catch (FileNotFoundException f)  {
	    throw new Exception("Couldn't find package " + packageName);
	}
	throw new Exception("Couldn't load package " + packageName);
    }

    /** Locates a package using the path enumeration. */
    public File locatePackage(String packageName, Enumeration paths) 
    {
	if (paths != null) {
	    while (paths.hasMoreElements()) {
		File file = new File((String) paths.nextElement(), packageName);
		if (file.exists() && file.isDirectory()) {
		    return file;
		}
	    }
	}
	return null;
    }

    /** Locates a package using the package path. */
    public File locatePackage(String packageName) throws FileNotFoundException {
	Enumeration paths;
	File pkg;
	paths = MaxApplication.enumerateProperty("pkg-path");
	pkg = locatePackage(packageName, paths);
	if (pkg == null) {
	    paths = MaxApplication.enumerateProperty("sys-pkg-path");
	    pkg = locatePackage(packageName, paths);
	    if (pkg == null) {
		throw new FileNotFoundException(packageName);
	    }
	}
	return pkg;
    }

    /** Returns the package with the given name. */
    public MaxPackage getPackage(String name) {
	return (MaxPackage) packageTable.get(name);
    }

    /** Returns an enumeration of the names of the packages known to
     *  the handler. */
    public Enumeration packageNames() {
	return packageTable.keys();
    }
}







