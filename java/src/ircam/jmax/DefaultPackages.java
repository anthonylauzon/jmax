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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 


package ircam.jmax;

import java.io.*;

//=====================================================
//   Platform 
//=====================================================

public class DefaultPackages
{
  
  public static void init()
  { 
      String fs = File.separator;
      String pkgPath = ((String)MaxApplication.getProperty("jmaxRoot"))+fs+"packages"+fs;      
      String path;
      Class pkgClass;

      try{
	  path = pkgPath+"guiobj"+fs+"java"+fs+"guiobj.jar";
	  PackageClassLoader.instance.setJarPath(path);
	  pkgClass = PackageClassLoader.instance.loadClass("ircam.jmax.guiobj.Guiobj", true);
	  ((JMaxPackage)pkgClass.newInstance()).load();
	  
	  path = pkgPath+"data"+fs+"java"+fs+"data.jar";
	  PackageClassLoader.instance.setJarPath(path);
	  pkgClass = PackageClassLoader.instance.loadClass("ircam.jmax.editors.bpf.Bpf", true);
	  ((JMaxPackage)pkgClass.newInstance()).load();
	  pkgClass = PackageClassLoader.instance.loadClass("ircam.jmax.editors.table.Table", true);
	  ((JMaxPackage)pkgClass.newInstance()).load();
      
	  path = pkgPath+"explode"+fs+"java"+fs+"explode.jar";
	  PackageClassLoader.instance.setJarPath(path);
	  pkgClass = PackageClassLoader.instance.loadClass("ircam.jmax.editors.explode.Explode", true);
	  ((JMaxPackage)pkgClass.newInstance()).load();

	  path = pkgPath+"ispw"+fs+"java"+fs+"ispw.jar";
	  PackageClassLoader.instance.setJarPath(path);
	  pkgClass = PackageClassLoader.instance.loadClass("ircam.jmax.ispw.Ispw", true);
	  ((JMaxPackage)pkgClass.newInstance()).load();
	  
	  path = pkgPath+"qlist"+fs+"java"+fs+"qlist.jar";
	  PackageClassLoader.instance.setJarPath(path);
	  pkgClass = PackageClassLoader.instance.loadClass("ircam.jmax.editors.qlist.Qlist", true);
	  ((JMaxPackage)pkgClass.newInstance()).load();
	  
	  path = pkgPath+"sequence"+fs+"java"+fs+"sequence.jar";
	  PackageClassLoader.instance.setJarPath(path);
	  pkgClass = PackageClassLoader.instance.loadClass("ircam.jmax.editors.sequence.Sequence", true);
	  ((JMaxPackage)pkgClass.newInstance()).load();
      }
      catch(IllegalAccessException e){
	  System.err.println("Error in Package loading: "+e);
      }
      catch(InstantiationException e){
	  System.err.println("Error in Package loading: "+e);
      }
      catch(ClassNotFoundException e){
	  System.err.println("Error in Package loading: "+e);
      }
  }
}
 
