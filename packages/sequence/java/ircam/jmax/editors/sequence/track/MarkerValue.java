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
public class MarkerValue extends AbstractEventValue
{
  public MarkerValue()
{
    super();
    
    setDefaultProperties();
}

private void setDefaultProperties()
{
	setProperty("duration", new Double(1.0));
  tempo = EventValue.UNKNOWN_PROPERTY;
  meter = EventValue.UNKNOWN_PROPERTY;
}

Object type, duration, tempo, meter;

public void setProperty(String name, Object value)
{
	if( name.equals("type"))
		type = value;
	else if(name.equals("duration"))
		duration = value;
	else if(name.equals("tempo"))
		tempo = value;
	else if(name.equals("meter"))
		meter = value;	
	else
		super.setProperty(name, value);	
}

public Object getProperty(String name)
{
	if(name.equals("type"))
	  return type;
	else if(name.equals("duration"))
	  return duration;
	else if(name.equals("tempo"))
	  return tempo;
	else if(name.equals("meter")) 
		return meter;
	else
		return super.getProperty(name);
}

public void unsetProperty( String name)
{
 if(name.equals("tempo"))
	  tempo = EventValue.UNKNOWN_PROPERTY;
	else if(name.equals("meter")) 
		meter = EventValue.UNKNOWN_PROPERTY;
	else
		super.unsetProperty(name);
}

public ValueInfo getValueInfo()
{
	return info;
}

public boolean isMovable()
{  
  return (getProperty("type").equals("marker") && getProperty("tempo") == EventValue.UNKNOWN_PROPERTY);
}

public String getPropertyMessage(String property)
{
  if(property.equals("tempo"))
    return "tempo_change";
  else if(property.equals("meter"))
    return "meter_change";
  else
    return property;
}

static class MarkerValueInfo extends AbstractValueInfo {
	/**
	* Returns the name of this value object */
	public String getName()
	{
		return SCOMARK_NAME;
	}
	
	public String getPublicName()
	{
		return SCOMARK_PUBLIC_NAME;
	}
	
	public ImageIcon getIcon()
	{
		return SCOMARK_ICON;
	}
	
	/**
	* Create an instance of the associated EventValue */
	public Object newInstance()
	{
		return new MarkerValue();
	}
	
	public DataFlavor getDataFlavor()
	{
		/*return MarkerValueDataFlavor.getInstance();*/
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
	return MarkerEventRenderer.getRenderer();
}

public void setPropertyValues(int nArgs, Object args[])
{
	/*if( nArgs == 2)
	{
		setProperty( "type", args[0]);
		setProperty( "tempo", args[1]);
	}
	else*/
		super.setPropertyValues( nArgs, args);
}

//--- Fields
public static final String fs = File.separator;
public static final String SCOMARK_NAME = "scomark";
public static final String SCOMARK_PUBLIC_NAME = "scomark";
static String path;
public static ImageIcon SCOMARK_ICON;
public static MarkerValueInfo info = new MarkerValueInfo();

static 
{
  if(JMaxApplication.getProperty("ftm") == null)
  {
    path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;
    SCOMARK_ICON = new ImageIcon(path+"seq_note.gif");
  }
  else
    SCOMARK_ICON = JMaxUtilities.loadIconFromResource("images/seq_note.gif");
}
}















