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

import java.awt.*;
import java.util.*;
import javax.swing.*;

import ircam.fts.client.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.objects.*;

public class ObjectCreatorManager
{
  static private ObjectCreatorManager creatorManager = new ObjectCreatorManager();

  public static ObjectCreatorManager getInstance()
  {
    return creatorManager;
  }

  public static void register(String nameclass, JMaxObjectCreator creator)
  {
    creatorManager.creatorsTable.put(nameclass, creator);

    /* to remove */
    creatorManager.classNamesVector.addElement(nameclass);
    //creatorManager.packageNames.put(nameclass, pkgName);
  }

  static public JMaxObjectCreator getCreator(String nameclass)
  {
    return ((JMaxObjectCreator)creatorManager.creatorsTable.get(nameclass));
  }

  private Hashtable creatorsTable = new Hashtable();


  /******************************************************************/
  /* to remove */
  static public String getPackageName(String nameclass)
  {
    return ((String)creatorManager.packageNames.get(nameclass));
  }
  
  static public Enumeration getClassNames()
  {
    return creatorManager.classNamesVector.elements();
  }  
  static public boolean containsClass(String className)
  {
    return  creatorManager.creatorsTable.containsKey(className);
  } 

  private Vector classNamesVector  = new Vector();
  private Hashtable packageNames   = new Hashtable();
}







