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

  ClassMapEntry( String ftsClassName, JMaxObjectCreator creator, String iconName, String cursorName, JMaxPackage jmaxPackage)
  {
    this.creator = creator;
    this.icon = JMaxUtilities.loadIconFromResource( iconName, jmaxPackage);
    if (cursorName != null)
      this.cursor = loadCursor( JMaxUtilities.loadIconFromResource( cursorName, jmaxPackage), ftsClassName);
  }

  ClassMapEntry( String ftsClassName, JMaxObjectCreator creator, String iconPath, String cursorPath)
  {
    this.creator = creator;
    this.icon = new ImageIcon( iconPath);
    if (cursorPath != null)
      this.cursor = loadCursor( new ImageIcon( cursorPath), ftsClassName);
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
}


public class JMaxClassMap {

  public static void put( String ftsClassName, JMaxObjectCreator creator, String iconName, String cursorName, JMaxPackage jmaxPackage)
  {
    instance.map.put( ftsClassName, new ClassMapEntry( ftsClassName, creator, iconName, cursorName, jmaxPackage));
    instance.names.add( ftsClassName);
  }

  public static void put( String ftsClassName, JMaxObjectCreator creator, String iconPath, String cursorPath)
  {
    instance.map.put( ftsClassName, new ClassMapEntry( ftsClassName, creator, iconPath, cursorPath));
    instance.names.add( ftsClassName);
  }

  public static void put( String ftsClassName, JMaxObjectCreator creator, String iconName, JMaxPackage jmaxPackage)
  {
    put( ftsClassName, creator, iconName, null, jmaxPackage);
  }

  public static void put( String ftsClassName, JMaxObjectCreator creator, String iconPath)
  {
    put( ftsClassName, creator, iconPath, (String)null);
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

