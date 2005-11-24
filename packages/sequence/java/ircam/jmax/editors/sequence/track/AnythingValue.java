//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.sequence.track;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;
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
  Object objId = new Integer(-1);
  Object varname = "";
  Object object  = null;
  
  public AnythingValue()
  {
    super();  
    setProperty("duration", new Double(64.0));
    setProperty("objid", objId);
  }
  
  public Object getProperty(String name)
  {
    if(name.equals("objid"))
      return objId;
    else if(name.equals("object"))
      return object;    
    else if(name.equals("name"))
      return varname;
    if(name.equals("type"))
      return type;
    else return super.getProperty(name);
  }

  public void setProperty( String name, Object value)
  { 
    if(name.equals("objid"))
    {
      if(((Integer)objId).intValue() != ((Integer)value).intValue())
      {
        objId = value;
        FtsObject obj = JMaxApplication.getFtsServer().getObject(((Integer)objId).intValue());
        setProperty("name", "#"+((Integer)objId).intValue());
        setProperty("object", obj);
      }
    }
    else if(name.equals("name"))
      this.varname = value;
    else if(name.equals("object"))
      this.object = value;    
    else if( name.equals("type"))
      type = (String)value;
    else 
      super.setProperty(name, value);
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
    public DataFlavor getDataFlavor()
    {
      return null;
    }
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

  //--- Fields
  public static final String fs = File.separator;
  public static final String ANYTHING_NAME = "any";
  public static final String ANYTHING_PUBLIC_NAME = "any";
  static String path;
  String type;
  public static ImageIcon ANYTHING_ICON;
  public static AnythingValueInfo info = new AnythingValueInfo();

  static 
  {
    if(JMaxApplication.getProperty("ftm") == null)
    {
      path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;  
      ANYTHING_ICON = new ImageIcon(path+"seq_anything.gif");
    }
    else
      ANYTHING_ICON = JMaxUtilities.loadIconFromResource("images/seq_anything.gif");
  }
}








