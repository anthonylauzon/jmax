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
import java.util.*;
import ircam.jmax.toolkit.*;
import javax.swing.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;

/**
 * A basic implementation of the EventValue interface, with a default property handling (no actions). 
 */
public class AbstractEventValue implements EventValue 
{

  public AbstractEventValue()
  {
  }

  public void setDataModel( TrackDataModel td)
  {
    model = td;
  }

  public TrackDataModel getDataModel()
  {
    return model;
  }
  /**
     * Set the named property */
  public void setProperty(String name, Object value)
  {
    if(properties==null)
      properties = new Hashtable();
    properties.put(name, value);
  }

  /**
   * Get the given property */
  public Object getProperty(String name)
  {
    if(properties==null)
      properties = new Hashtable();
    
    if (properties.containsKey(name))
      return properties.get(name);
    else return UNKNOWN_PROPERTY; //to be overloaded
  }
    
  public void edit(int x, int y, int modifiers, Event evt, SequenceGraphicContext gc){}

  public JPopupMenu getPopupMenu()
  {
    return null;
  }

  public String[] getLocalPropertyNames()
  {
    return new String[0];
  }

  public int getLocalPropertyCount()
  {
    return 0;
  }

  public static Object[] propertyValuesArray = new Object[128];
  public Object[] getPropertyValues()
  {
    int i = 0;
    Object prop;
    for(Enumeration e = model.getPropertyNames(); e.hasMoreElements();)
      {
	prop = getProperty( (String)e.nextElement());
	if( prop != UNKNOWN_PROPERTY)
	  propertyValuesArray[i++] = prop;
      }      
    return propertyValuesArray;
  }
  public void setPropertyValues(int nArgs, Object args[])
  {

    System.err.println("setPropertyValues nArgs = "+nArgs);
    for(int i =0; i<nArgs;i++)
      System.err.println("args["+i+"] = "+args[i]);

    int i = 0;
    for(Enumeration e = model.getPropertyNames(); e.hasMoreElements();)
      setProperty( (String)e.nextElement(), args[i++]);
  }
  
  public int getDefinedPropertyCount()
  {
    int i = 0;
    for(Enumeration e = model.getPropertyNames(); e.hasMoreElements();)
      if( getProperty( (String)e.nextElement()) != UNKNOWN_PROPERTY) i++;
  
    return i;
  }
  public boolean samePropertyValues(Object args[])
  {
    return false;
  }

    public Object[] getLocalPropertyValues()
    {
	return propertyValuesArray;
    }
    public void setLocalPropertyValues(int nArgs, Object args[])
    {
    }

    /**
     * Returns the renderer for this object */
    public SeqObjectRenderer getRenderer()
    {
	return null; 
    }

    public ValueInfo getValueInfo()
    {
	return AbstractValueInfo.instance;
    }


    /**
     * A convenience class to implement an empty enumeration */
    public class EmptyEnumeration implements Enumeration {
      public boolean hasMoreElements()
      {
	return false;
      }
      
      public Object nextElement()
      {
	return null;
      }
    }

  //--- Fields
  private String name;
  protected Hashtable properties;
  TrackDataModel model;
}







