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

package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.fts.client.*;
import ircam.jmax.*;

public class FtsProject extends FtsPackage
{
  public FtsProject() throws IOException
  {
    super();
  }

  public FtsProject (FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);
  }

  void addPackage( String pkgName, int id)
  {
    super.addPackage( pkgName, id);
    loadPackage( pkgName, id);
  }

  void loadPackage( String pkgName, int id)
  {
    FtsPackage ftsPkg;
	
    if( !packages.containsKey( pkgName))
      {
	System.out.println( "package: " + pkgName);  

	try
	  {
	    JMaxPackageLoader.load( pkgName);
	  }
	catch( JMaxPackageLoadingException e)
	  {
	    JMaxApplication.reportException( e);
	  }	    
	    
	ftsPkg = new FtsPackage( JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), id);
	packages.put(pkgName, ftsPkg);
	ftsPkg.upload();
      }
  }

  Hashtable packages = new Hashtable();
}

