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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
public class MidiValue extends AbstractEventValue
{
    public MidiValue()
    {
	super();

	setProperty("integer", new Integer(0));
	setProperty("number", new Integer(0));
	setProperty("channel", new Integer(1));
	setProperty("duration", new Double(64.0));
    }

  Object intValue, channel, number, duration;

  public void setProperty(String name, Object value)
    {
	if(name.equals("integer"))
	    intValue = value;
	else if(name.equals("number"))
	    number = value;
	else if(name.equals("channel"))
	    channel = value;
	else if(name.equals("duration"))
	    duration = value;
	else super.setProperty(name, value);
  }
  public Object getProperty(String name)
  {
      if(name.equals("integer"))
	  return intValue;
      if(name.equals("number"))
	  return number;
      if(name.equals("channel"))
	  return channel;
      if(name.equals("duration"))
	  return duration;
      else return super.getProperty(name);
  }

    public ValueInfo getValueInfo() 
    {
	return info;
    }

    static class MidiValueInfo extends AbstractValueInfo {
	/**
	 * Returns the name of this value object */
	public String getName()
	{
	    return MIDI_NAME;
	}

	public String getPublicName()
	{
	    return MIDI_PUBLIC_NAME;
	}

	public ImageIcon getIcon()
	{
	    return MIDI_ICON;
	}

	public Object newInstance()
	{
	    return new MidiValue();
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
	    return IntegerValueDataFlavor.getInstance();
	}
	public Class getPropertyType(int index)
	{
	    /*if(index < defPropertyCount)
	      return propertyTypesArray[index];
	      else
	      return Integer.class;*/
	    return Integer.class;
	}

	String defNamesArray[] = {"integer", "number", "channel"};
	//Class propertyTypesArray[] = {Integer.class, Integer.class, Integer.class};
	int defPropertyCount = 3;
    }

    /**
     * Returns its specialized renderer (an AmbitusEventRenderer) */
    public SeqObjectRenderer getRenderer()
    {
	return IntegerEventRenderer.getRenderer();
    }
  
    public Enumeration getPropertyNames()
    {
	return new ArrayEnumeration(nameArray);
    }

    public int getPropertyType(int index)
    {
	if(index < propertyCount)
	    return propertyTypes[index];
	else return UNKNOWN_TYPE;
    }
    
    public int getPropertyCount()
    {
	return propertyCount;
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
	return (((Integer)propertyValuesArray[0]).intValue() == ((Integer)args[0]).intValue() &&
		((Integer)propertyValuesArray[1]).intValue() == ((Integer)args[1]).intValue() &&
		((Integer)propertyValuesArray[2]).intValue() == ((Integer)args[2]).intValue());
    }

    //--- Fields
    public static final String fs = File.separator;
    public static final String MIDI_NAME = "midival";
    public static ImageIcon MIDI_ICON; 
    static String path;
    public static final String MIDI_PUBLIC_NAME = "midi";
    public static MidiValueInfo info = new MidiValueInfo();
    public static final int DEFAULT_MAX_VALUE = 127;
    public static final int DEFAULT_MIN_VALUE = 0;

    static String nameArray[] = {"integer", "number", "channel"};
    static int propertyTypes[] = {INTEGER_TYPE, INTEGER_TYPE, INTEGER_TYPE};
    static int propertyCount = 3;

    static 
    {
	try
	    {
		path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+fs+"images"+fs;
	    }
	catch(FileNotFoundException e){
	    path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
	}
	MIDI_ICON = new ImageIcon(path+"midi.gif");
    }
}
