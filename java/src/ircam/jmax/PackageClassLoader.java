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

package ircam.jmax;

import java.util.*;
import java.io.*;
import java.util.zip.*;

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
    protected JMaxPackage itsPackage;

    protected String jarPath;

    /** Constructs a new package class loader. */
    public PackageClassLoader() 
    {
	loadedClasses = new Hashtable();
    }

    public void setJarPath(String path)
    {
	jarPath = path;
    }

    /** Loads a class. The package is used to locate the file. If the
     *  file isn't found in the package, the system is asked to load
     *  the package. */
    //public synchronized Class loadClass(String jarPath, String className) 
    public synchronized Class loadClass(String className, boolean resolveIt) throws ClassNotFoundException
    {
	Class claz = (Class) loadedClasses.get(className);
	if (claz == null) {
	    try {
		String nName = className.replace ('.', File.separatorChar) + ".class";
		byte[] data = getClassFromJar(jarPath, nName);
		
		if(data!=null)
		{
		    claz = defineClass(className, data, 0, data.length);
		    loadedClasses.put(className, claz);
		}
		else
		    return super.loadClass(className, resolveIt); 
	    }
	    catch (IOException e) {
		throw new ClassNotFoundException("IO exception while reading class data "+className);
	    }
	}

	if(resolveIt)
	    resolveClass(claz);

	return claz;
    }

    public static PackageClassLoader instance = new PackageClassLoader();
    
    private byte[] getClassFromJar(
				   String jarName,     // An absoulte path for a jar file to search.
				   String className)   // The name of the class to extract from the jar file.
	throws IOException
    {
	ZipInputStream zin;        // The jar file input stream.
	ZipEntry       entry;      // A file contained in the jar file.
	byte[]         result;     // The bytes that compose the class file.
	int            size;       // Uncompressed size of the class file.
	int            total;      // Number of bytes read from class file.

	zin = new ZipInputStream(new FileInputStream(jarName));
	
	try {
	    while ((entry = zin.getNextEntry()) != null) { 
		// see if the current ZipEntry's name equals 
		// the file we want to extract. If equal
		// get the extract and return the contents of the file.
	      
		if (className.equals(entry.getName())) {
		    size = getEntrySize(jarName, className);
		    result = new byte[size];
		    total = zin.read(result);
		    while (total != size) {
			total += zin.read(result, total, 
					  (size - total));
		    }
		    return result;
		}
	    }
	    return null;
	} finally {
	    zin.close();
	}
    }
    private int getEntrySize(String jarName, String className) throws IOException
    {
	ZipEntry       entry;      // A file contained in the jar file.    
	ZipFile        zip;        // Used to get the enum of ZipEntries.  
	Enumeration    enum;       // List of the contents of the jar file.
	
	zip = new ZipFile(jarName);
	enum = zip.entries();

	while (enum.hasMoreElements()) { 
	    // see if the current ZipEntry's
	    // name equals the file we want to extract.
	  
	    entry = (ZipEntry) enum.nextElement();
	    if (className.equals(entry.getName())) {
		zip.close();
		return((int) entry.getSize());
	    }
	}
	return(-1);
    }
}




