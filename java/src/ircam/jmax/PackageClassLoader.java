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

package ircam.jmax;

import java.io.*;
import java.util.jar.*;
import java.util.zip.*;

/**
 * PackageClassLoader
 * 
 * The class loader for the jMax packages.
 * It simply load the class from a jar file.
 */
class PackageClassLoader extends ClassLoader {

  PackageClassLoader( String jarPath) throws IOException, FileNotFoundException
  {
    File f = new File( jarPath);

    if ( ! (f.exists() && f.isFile()) )
      throw new FileNotFoundException();

    jarFile = new JarFile( jarPath);
  }

  // Note from the Java 2 API documentation about the method loadClass of the class java.lang.ClassLoader:
  // "From the Java 2 SDK, v1.2, subclasses of ClassLoader are encouraged to override
  // findClass(String), rather than this method."

  protected Class findClass( String name) throws ClassNotFoundException
  {
    byte[] b = loadClassData( name);

    Class c = defineClass( name, b, 0, b.length);

    resolveClass( c);

    return c;
  }

  public InputStream getResourceAsStream( String name)
  {
    ZipEntry zipEntry = jarFile.getEntry( name);

    if (zipEntry == null)
      return null;

    try
      {
	return jarFile.getInputStream( zipEntry);
      }
    catch( IOException e)
      {
      }

    return null;
  }

  private byte[] loadClassData( String name) throws ClassNotFoundException
  {
    String entryName = name.replace( '.', '/') + ".class";
    ZipEntry zipEntry = jarFile.getEntry( entryName);

    if (zipEntry == null)
      throw new ClassNotFoundException( "Cannot find JAR file entry " + "\"" + entryName + "\"");

    int size = (int)zipEntry.getSize();
    byte b[] = new byte[ size];

    try
      {
	InputStream is = jarFile.getInputStream( zipEntry);
	int offset = 0;

	do
	  {
	    int n = is.read( b, offset, size);
	    offset += n;
	    size -= n;
	  }
	while ( size > 0);
      }
    catch( IOException e)
      {
	throw new ClassNotFoundException( "Cannot read JAR file entry " + "\"" + entryName + "\"", e);
      }

    return b;
  }

  private JarFile jarFile;
}
