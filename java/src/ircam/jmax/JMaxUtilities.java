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
import java.util.*;

// import javax.swing.*;
import javax.swing.ImageIcon;

public class JMaxUtilities {
  public static ImageIcon loadIconFromResource( String name, JMaxPackage jmaxPackage)
  {
    return loadAux( jmaxPackage.getClass().getResourceAsStream( name));
  }

  public static ImageIcon loadIconFromResource( String name)
  {
    return loadAux( ClassLoader.getSystemResourceAsStream( name));
  }

  private static ImageIcon loadAux( InputStream in)
  {
    if (in == null)
      return null;

    int n;
    ByteArrayOutputStream buffer = new ByteArrayOutputStream();
    byte[] b = new byte[4096];
    
    try
      {
	do
	  {
	    n = in.read( b);
	    if ( n <= 0)
	      break;
	    buffer.write( b, 0, n);
	  }
	while ( n > 0);
      }
    catch( IOException e)
      {
	return null;
      }

    return new ImageIcon( buffer.toByteArray());
  }
}

