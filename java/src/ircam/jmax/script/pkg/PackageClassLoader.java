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
 *  PackageClassLoader
 * 
 *  The class loader for the package management system.
 */
public class PackageClassLoader extends ClassLoader {

    /** The table containing all the classes the package handler
     *  loaded. */
    private Hashtable loadedClasses;

    /** The package this loader is working for. */
    protected Package itsPackage;

    /** Constructs a new package class loader. */
    public PackageClassLoader(Package pkg) 
    {
	itsPackage = pkg;
	loadedClasses = new Hashtable();
    }

    /** Loads a class. The package is used to locate the file. If the
     *  file isn't found in the package, the system is asked to load
     *  the package. */
    public synchronized Class loadClass(String name, boolean resolve) throws ClassNotFoundException
    {
	Class claz = (Class) loadedClasses.get(name);
	if (claz == null) {
	    try {
		File classfile;
		if (MaxApplication.getProperty("use-def-files") != null) {
		    classfile = itsPackage.locateClass(name);
		} else {
		    classfile = itsPackage.locateFile(name, ".class");
		}
		FileInputStream in = new FileInputStream(classfile);
		byte[] data = new byte[in.available()];
		in.read(data);
		claz = defineClass(data, 0, data.length);
		loadedClasses.put(name, claz);
	    } catch (FileNotFoundException e) {
		claz = findSystemClass(name);
	    } catch (ClassNotFoundException e) {
		claz = findSystemClass(name);
	    } catch (IOException e) {
		throw new ClassNotFoundException("IO exception while reading class data");
	    }
	}
	if (resolve) {
	    resolveClass(claz);
	}
	return claz;
    }
}

