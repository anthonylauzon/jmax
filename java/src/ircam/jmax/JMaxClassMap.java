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
import java.awt.image.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

class ClassMapEntry {

  ClassMapEntry( String ftsClassName, JMaxObjectCreator creator, String iconName, String cursorName, String description, JMaxPackage jmaxPackage)
  {
    this.creator = creator;
    if (iconName != null)
      this.icon = JMaxUtilities.loadIconFromResource( iconName, jmaxPackage);
    if (cursorName != null)
      this.cursor = loadCursor( JMaxUtilities.loadIconFromResource( cursorName, jmaxPackage), ftsClassName);
    this.description = description;
  }

  ClassMapEntry( String ftsClassName, JMaxObjectCreator creator, String iconPath, String cursorPath, String description)
  {
    this.creator = creator;
    if (iconPath != null)
      this.icon = new ImageIcon( iconPath);
    if (cursorPath != null)
      this.cursor = loadCursor( new ImageIcon( cursorPath), ftsClassName);
    this.description = description;
  }

  private Cursor loadCursor( ImageIcon cursorIcon, String description)
  {
    if (cursorIcon == null)
      return null;

    Image image = cursorIcon.getImage();
    ImageObserver observer = new ImageObserver() {
	public boolean imageUpdate( Image img, int infoflags, int x, int y, int width, int height)
	{
	  return false;
	}
      };
    int imageWidth = image.getWidth(observer);
    int imageHeight = image.getHeight(observer);

    Dimension bestSize = Toolkit.getDefaultToolkit().getBestCursorSize( imageWidth, imageHeight);
    BufferedImage bi = new BufferedImage( bestSize.width, bestSize.height, BufferedImage.TYPE_INT_ARGB);
    bi.createGraphics().drawImage( image, 0, 0, observer);
		
    Point hs;
    if ( bi.getHeight( observer) < 1)
      hs = new Point(0,0);
    else
      hs = new Point(0, 1);
      
    return Toolkit.getDefaultToolkit().createCustomCursor( bi, hs, description + " cursor");
  }

  JMaxObjectCreator creator;
  ImageIcon icon;
  Cursor cursor;
  String description;
}


public class JMaxClassMap {

  public static void put( String ftsClassName, JMaxObjectCreator creator, String iconName, String cursorName, String description, JMaxPackage jmaxPackage)
  {
    instance.map.put( ftsClassName, new ClassMapEntry( ftsClassName, creator, iconName, cursorName, description, jmaxPackage));
    if(( iconName != null) && ( cursorName != null))
      instance.names.add( ftsClassName);
  }

  public static void put( String ftsClassName, JMaxObjectCreator creator, String iconPath, String cursorPath, String description)
  {
    instance.map.put( ftsClassName, new ClassMapEntry( ftsClassName, creator, iconPath, cursorPath, description));
    if(( iconPath != null) && ( cursorPath != null))
      instance.names.add( ftsClassName);
  }

  public static void put( String ftsClassName, JMaxObjectCreator creator, String iconName, String description, JMaxPackage jmaxPackage)
  {
    put( ftsClassName, creator, iconName, null, description, jmaxPackage);
  }

  public static void put( String ftsClassName, JMaxObjectCreator creator, String iconPath, String description)
  {
    put( ftsClassName, creator, iconPath, (String)null, description);
  }

  public static JMaxObjectCreator getCreator( String ftsClassName)
  {
    if(instance.map.containsKey(ftsClassName))
      return ((ClassMapEntry)instance.map.get( ftsClassName)).creator;
    else return null;
  }

  public static ImageIcon getIcon( String ftsClassName)
  {
    if(instance.map.containsKey(ftsClassName))
      return ((ClassMapEntry)instance.map.get( ftsClassName)).icon;
    else return null;
  }

  public static Cursor getCursor( String ftsClassName)
  {
    if(instance.map.containsKey(ftsClassName))
      return ((ClassMapEntry)instance.map.get( ftsClassName)).cursor;
    else return null;
  }

  public static String getDescription( String ftsClassName)
  {
    if(instance.map.containsKey(ftsClassName))
      return ((ClassMapEntry)instance.map.get( ftsClassName)).description;
    else return null;
  }

  public static Iterator getClassNames()
  {
    return instance.names.iterator();
  }

  private JMaxClassMap()
  {
    map = new HashMap();
    names = new ArrayList();
  }

  private static JMaxClassMap instance = new JMaxClassMap();
  private HashMap map;
  private ArrayList names;
}

