//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax;

import java.awt.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.objects.*;

public class ObjectCreatorManager
{
  static private ObjectCreatorManager creatorManager = new ObjectCreatorManager();

  public static ObjectCreatorManager getInstance()
  {
    return creatorManager;
  }

  /****************************************************************************/

  public static void registerFtsClass(String nameclass, Class theClass)
  {
     creatorManager.ftsClasses.put(nameclass, theClass);
  }
  public static void registerGraphicClass(String nameclass, Class theClass, String pkgName)
  {
      creatorManager.graphicClasses.put(nameclass, theClass);
      creatorManager.classNamesVector.addElement(nameclass);
      creatorManager.packageNames.put(nameclass, pkgName);
  }
  static public Class getFtsClass(String nameclass)
  {
      return ((Class)creatorManager.ftsClasses.get(nameclass));
  }
  static public Class getGraphicClass(String nameclass)
  {
    return ((Class)creatorManager.graphicClasses.get(nameclass));
  } 
  static public String getPackageName(String nameclass)
  {
    return ((String)creatorManager.packageNames.get(nameclass));
  }
  static public Enumeration getGraphicClasses()
  {
      return creatorManager.graphicClasses.elements();
  }
  static public Enumeration getClassNames()
  {
      return creatorManager.classNamesVector.elements();
  }  

  private Hashtable ftsClasses     = new Hashtable();
  private Hashtable graphicClasses = new Hashtable();
  private Vector classNamesVector  = new Vector();
  private Hashtable packageNames   = new Hashtable();
}







