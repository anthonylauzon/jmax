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

package ircam.jmax.editors.sequence.track;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import javax.swing.*;
import java.awt.*;
import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;

/**
 * The EventValue object that represents a midi-like note with "ambitus" information */
public class AnythingValue extends AbstractEventValue
{
  public AnythingValue()
  {
    super();
  
    setProperty("duration", new Double(1));
  }
  
  public ValueInfo getValueInfo()
  {
    return info;
  }
  
  static class AnythingValueInfo extends AbstractValueInfo {
    /**
     * Returns the name of this value object */
    public String getName()
    {
      return ANYTHING_NAME;
    }

    public String getPublicName()
    {
      return ANYTHING_PUBLIC_NAME;
    }
    
    public ImageIcon getIcon()
    {
      return ANYTHING_ICON;
    }
    
    /**
     * Create an instance of the associated EventValue */
    public Object newInstance()
    {
      return new AnythingValue();
    }

    public Enumeration getPropertyNames()
    {
      return new ArrayEnumeration(defNamesArray);
    }


    public int getPropertyCount()
    {
      return defPropertyCount;
    }

    public DataFlavor getDataFlavor()
    {
	//return AnythingValueDataFlavor.getInstance();
	return null;
    }

    public Class getPropertyType(int index)
    {
	return Integer.class;
    }
 
    String defNamesArray[] = {};
    int defPropertyCount = 0;
  }

  public JPopupMenu getPopupMenu()
  {
    return null;
  } 

  /**
   * Returns its specialized renderer (an AmbitusEventRenderer) */
  public SeqObjectRenderer getRenderer()
  {
    return AnythingEventRenderer.getRenderer();
  }
  
  public Enumeration getPropertyNames()
  {
    return new ArrayEnumeration(nameArray);
  }
  public int getPropertyCount()
  {
    return propertyCount;
  }

  public int getPropertyType(int index)
  {
    if(index < propertyCount)
      return propertyTypes[index];
    else return UNKNOWN_TYPE;
  }

  public Object[] getPropertyValues()
  {
    for(int i = 0; i<propertyCount; i++)
      propertyValuesArray[i] = getProperty(nameArray[i]);

    return propertyValuesArray;
  }

  public void setPropertyValues(int nArgs, Object args[])
  {
    for(int i = 0; i<nArgs; i++)
      setProperty(nameArray[i], args[i]);
  }

  public boolean samePropertyValues(Object args[])
  {
      return true;
  }

  //--- Fields
  public static final String fs = File.separator;
  public static final String ANYTHING_NAME = "any";
  public static final String ANYTHING_PUBLIC_NAME = "any";
  static String path;
  public static ImageIcon ANYTHING_ICON;
  public static AnythingValueInfo info = new AnythingValueInfo();

  static 
  {
      /*
	WARNING:
	Waiting for a method to get the packagePath from the package name
      */
      /*try
	{
	path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+fs+"images"+fs;
	}
	catch(FileNotFoundException e){
	path = JMaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
	}*/
      path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;//??????????????   
      /*************************************************************/
      
      ANYTHING_ICON = new ImageIcon(path+"anything.gif");
  }

  static String nameArray[] = {};
  static int propertyTypes[] = {};
  static int propertyCount = 0;
}








