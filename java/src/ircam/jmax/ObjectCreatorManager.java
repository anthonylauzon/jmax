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

  public static void registerFtsObjectCreator(String nameclass, FtsObjectCreator creator)
  {
      creatorManager.ftsCreators.put(nameclass, creator);
  }
  public static void registerGraphicObjectCreator(String nameclass, GraphicObjectCreator creator)
  {
      //Icons.loadIcon(nameclass, creator.getIconName());
      creatorManager.graphicCreators.put(nameclass, creator);
  }
  static public FtsObjectCreator getFtsObjectCreator(String nameclass)
  {
    return ((FtsObjectCreator)creatorManager.ftsCreators.get(nameclass));
  }
  static public GraphicObjectCreator getGraphicObjectCreator(String nameclass)
  {
    return ((GraphicObjectCreator)creatorManager.graphicCreators.get(nameclass));
  }
  static public Enumeration getGraphicCreators()
  {
      return creatorManager.graphicCreators.elements();
  }

  private Hashtable ftsCreators = new Hashtable();
  private Hashtable graphicCreators = new Hashtable();
}







