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

/** PackageHandler
 * 
 *  A very primitive implementation of a package system.
 */
package ircam.jmax.script.pkg;
import ircam.jmax.*;
import ircam.jmax.script.*;
import java.util.*;
import java.io.*;

public class PackageHandler {
    /** All the packages that the handler knows about are stored in the
     *  packageTable */
    private Hashtable packageTable;

    /** A string containing all the paths used to locate a
     *  package. Unfortunately, the file separator and the path
     *  separator are system dependend. */
    // FIXME should become an application property.
    private String itsPkgPath;

    /** This script is executed as a last resort after the handler
     *  failed to load a package. */
    private Script unknownScript;

    /** Constructs a new PackageHandler. */
    public PackageHandler() 
    {
	packageTable = new Hashtable();
    }

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

    /** Indicate that the package has been loaded and initialized. If
     *  the package is not yet known to the handler, a new entry in
     *  the table is created. */
    public void provide(String packageName, String version) 
    {
	Package pkg = (Package) packageTable.get(packageName);
	if (pkg == null) {
	    pkg = new Package(this, packageName, version);
	    packageTable.put(packageName, pkg);
	}
	pkg.setLoaded(true);
    }

    /** Insert the package in the table is it isn't there already. The
     *  package is not marked as being loaded. */
    public void insert(String packageName, String version) 
    {
	Package pkg = (Package) packageTable.get(packageName);
	if (pkg == null) {
	    pkg = new Package(this, packageName, version);
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
	    System.out.println("Error loading package " + packageName + " : package not found");
	}
    }

    /** Makes sure that a package is loaded. If the package is not
     *  loaded or known the package's load script and/or the unknown
     *  script are used to load the package. */
    public void require(String packageName, String version) throws Exception
    {
	Package pkg = (Package) packageTable.get(packageName);
	File pkgPath = null;
	try {
		    
	    if (pkg != null) {
		if (pkg.isLoaded()) {
		    pkg.requireVersion(version);
		    return;
		} else {
		    
		    pkgPath = locatePackage(packageName);
		    
		    if (pkgPath != null) {
			pkg.load(pkgPath, version, MaxApplication.getInterpreter());
			return;
		    } else if (unknownScript != null) {
			
			unknownScript.eval(packageName, version);
			if (pkg.isLoaded()) {
			    /* In case the unknown script loaded the package. */
			    pkg.requireVersion(version);
			    return;
			} else {
			    /* In case the unknown script added a new package path. */
			    pkgPath = locatePackage(packageName);
			    if (pkgPath != null) {
				pkg.load(pkgPath, version, MaxApplication.getInterpreter());
				return;
			    }
			}
		    }
		}
	    } else {
		/* Keep an entry of the package in the table. */ 
		insert(packageName, version);

		/* And start al over again. */
		require(packageName, version);
		
		return;
	    }
	} catch (ScriptException e)  {
	    throw new Exception(e.getMessage());
	}
	throw new Exception("Couldn't load package " + packageName);
    }

    /** Locates a package using the package path. */
    public File locatePackage(String packageName) throws FileNotFoundException
    {
	if (itsPkgPath != null) {
	    StringTokenizer st = new StringTokenizer(itsPkgPath, File.pathSeparator);
	    while (st.hasMoreTokens()) {
		String path = st.nextToken() + File.separator + packageName;
		File packageDir = new File(path);
		if (packageDir.exists()) {
		    return packageDir;
		}
	    }
	}
	throw new FileNotFoundException(packageName);
    }

    /** Appends a path to the package path. */
    public void appendPath(String path) 
    {
	if (itsPkgPath == null) {
	    itsPkgPath = path;
	} else {
	    itsPkgPath += File.pathSeparator + path;
	}
    }

    /** Returns an enumeration of the names of the packages known to
     *  the handler. */
    public Enumeration packageNames() {
	return packageTable.keys();
    }
}







