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
import ircam.jmax.editors.project.*;

public class FtsProject extends FtsPackage
{
  static
  {
    FtsObject.registerMessageHandler( FtsProject.class, FtsSymbol.get("config"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  String config = null;
	  if( args.getLength() == 1)
	    config = args.getSymbol( 0).toString();
	  
	  ((FtsProject)obj).setCurrentConfig( config);
	}
      });
  }

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

  public void setAsCurrentProject()
  {
    try
      {
	send( FtsSymbol.get("set_as_current_project"));
      }
    catch(IOException e)
      {
	System.err.println("FtsProject: I/O Error sending setAsCurrentProject Message!");
	e.printStackTrace(); 
      }
  }

  public void saveAsDefault()
  {
    try
      {
	send( FtsSymbol.get("save_as_default"));
      }
    catch(IOException e)
      {
	System.err.println( "FtsProject: I/O Error sending saveAsDefault Message!");
	e.printStackTrace(); 
      }
  }

  public void setConfig( String fileName)
  {
    args.clear();

    if( fileName != null)
      args.addSymbol( FtsSymbol.get( fileName));
    try
      {
	send( FtsSymbol.get("config"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsProject: I/O Error sending setConfig Message!");
	e.printStackTrace(); 
      }
  }

  public void setCurrentConfig( String fileName)
  {
    config = fileName;
    if( listener != null)
      listener.configChanged( fileName);
  }

  public String getConfig()
  {
    return config;
  } 

  public void saveWindows( Enumeration windows)
  {
    String name;
    FtsPatcherObject patch;

    args.clear();

    for(Enumeration e = windows; e.hasMoreElements();)
      {      
	patch = (FtsPatcherObject)e.nextElement();
	args.addSymbol( FtsSymbol.get( patch.getName()));
	/*args.addInt( patch.getX());
	  args.addInt( patch.getY());*/
      }
    try
      {
	send( FtsSymbol.get("save_windows"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsProject: I/O Error sending saveWindows Message!");
	e.printStackTrace(); 
      }
  }

  Hashtable packages = new Hashtable();
  String config = null;
  public static Vector alreadyLoaded = new Vector();
}

