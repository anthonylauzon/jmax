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
public class AmbitusValue extends AbstractEventValue
{
  public AmbitusValue()
  {
    super();
    
    setDefaultProperties();
  }
  
  private void setDefaultProperties()
  {
    setProperty("duration", new Double(100.0));
    //setProperty("pitch", new Integer(0));
  }

  Object pitch, duration, velocity, channel, ambitus;
  
  public void setProperty(String name, Object value)
  {
    if(name.equals("pitch"))
      {
	if(value instanceof Double)
	  value = new Integer( ((Double)value).intValue());
	
	if(((Integer)value).intValue() > 127)
	  value = new Integer(127);
	pitch = value;
      }
    else if(name.equals("duration"))
      duration = value;
    else if(name.equals("velocity"))
      velocity = value;
    else if(name.equals("channel"))
      channel = value;
    //else
    super.setProperty(name, value);
  }
  public Object getProperty(String name)
  {
      if(name.equals("pitch"))
	  return pitch;
      else if(name.equals("duration"))
	  return duration;
      else if(name.equals("velocity"))
	  return velocity;
      else if(name.equals("channel"))
	  return channel;
      else
	return super.getProperty(name);
  }

  public ValueInfo getValueInfo()
  {
    return info;
  }
  
  static class AmbitusValueInfo extends AbstractValueInfo {
    /**
     * Returns the name of this value object */
    public String getName()
    {
      return AMBITUS_NAME;
    }

    public String getPublicName()
    {
      return AMBITUS_PUBLIC_NAME;
    }
    
    public ImageIcon getIcon()
    {
      return AMBITUS_ICON;
    }
    
    /**
     * Create an instance of the associated EventValue */
    public Object newInstance()
    {
      return new AmbitusValue();
    }

    public DataFlavor getDataFlavor()
    {
      return AmbitusValueDataFlavor.getInstance();
    }
  }

  public JPopupMenu getPopupMenu()
  {
    //return MidiEventPopupMenu.getInstance();
    return null;
  } 

  /**
   * Returns its specialized renderer (an AmbitusEventRenderer) */
  public SeqObjectRenderer getRenderer()
  {
    return AmbitusEventRenderer.getRenderer();
  }
  
  public void setPropertyValues(int nArgs, Object args[])
  {
    if( nArgs == 2)
      {
	setProperty( "pitch", args[0]);
	setProperty( "duration", args[1]);
      }
    else
      super.setPropertyValues( nArgs, args);
  }

  public boolean samePropertyValues(Object args[])
  {
    return ((((Integer)getProperty("pitch")).intValue() == ((Integer)args[0]).intValue()) &&
	    (((Double)getProperty("duration")).floatValue() == ((Double)args[1]).floatValue()) &&
	    (((Integer)getProperty("velocity")).intValue() == ((Integer)args[2]).intValue()) &&
	    (((Integer)getProperty("channel")).intValue() == ((Integer)args[3]).intValue()));
  }

  //--- Fields
  public static final int DEFAULT_MAX_PITCH = 127;
  public static final int DEFAULT_MIN_PITCH = 0;
  public static final String fs = File.separator;
  public static final String AMBITUS_NAME = "scoob";
  public static final String AMBITUS_PUBLIC_NAME = "scoob";
  static String path;
  public static ImageIcon AMBITUS_ICON;
  public static AmbitusValueInfo info = new AmbitusValueInfo();

  static 
  {
    path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;
    AMBITUS_ICON = new ImageIcon(path+"note.gif");
  }
}















