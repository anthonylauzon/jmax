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

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.*;
import java.io.*;
import javax.swing.*;
import java.util.*;
import java.awt.datatransfer.*;
/**
 * The EventValue object that represents a Integer event. Is used during score-recognition */
public class IntegerValue extends AbstractEventValue
{
  public IntegerValue()
  {
    super();

    setProperty("value", new Integer(0));
    setProperty("duration", new Double(64.0));
  }

  Object intValue, duration;
  public void setProperty(String name, Object value)
  {
    if(name.equals("value"))
      intValue = value;
    else if(name.equals("duration"))
      duration = value;
    else
      super.setProperty(name, value);
  }
  public Object getProperty(String name)
  {
    if(name.equals("value"))
      return intValue;
    else if(name.equals("duration"))
      return duration;
    else
      return super.getProperty(name);
  }

  public ValueInfo getValueInfo() 
  {
    return info;
  }

  static class IntegerValueInfo extends AbstractValueInfo {
    /**
     * Returns the name of this value object */
    public String getName()
    {
      return INTEGER_NAME;
    }

    public String getPublicName()
    {
      return INTEGER_PUBLIC_NAME;
    }
    
    public ImageIcon getIcon()
    {
      return INTEGER_ICON;
    }
    
    public Object newInstance()
    {
      return new IntegerValue();
    }
    public DataFlavor getDataFlavor()
    {
      return IntegerValueDataFlavor.getInstance();
    }
  }

  /**
   * Returns its specialized renderer (an AmbitusEventRenderer) */
  public SeqObjectRenderer getRenderer()
  {
    return IntegerEventRenderer.getRenderer();
  }
   
  public void setPropertyValues(int nArgs, Object args[])
  {
    if( nArgs == 1)
      setProperty( "value", args[0]);
    else
      super.setPropertyValues(nArgs, args);
  }

  public boolean samePropertyValues(Object args[])
  {
    return (((Integer)propertyValuesArray[0]).intValue() == ((Integer)args[0]).intValue());
  }

  //--- Fields
  public static final String fs = File.separator;
  public static final String INTEGER_NAME = "int";
  public static ImageIcon INTEGER_ICON; 
  static String path;
  public static final String INTEGER_PUBLIC_NAME = "integer";
  public static IntegerValueInfo info = new IntegerValueInfo();
  public static final int DEFAULT_MAX_VALUE = 127;
  public static final int DEFAULT_MIN_VALUE = 0;
  
  static 
  {
    path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;
    INTEGER_ICON = new ImageIcon(path+"integer.gif");
  }
}



