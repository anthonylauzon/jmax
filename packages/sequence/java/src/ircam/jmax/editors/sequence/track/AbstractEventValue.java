
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
public class AbstractEventValue extends FtsRemoteData implements EventValue 
{

    public AbstractEventValue()
    {
    }

    public void call(int id, FtsStream stream)
    {
	// to be implemented
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
    /**
     * This implementation returns an EmptyEnumeration */
    public Enumeration getPropertyNames()
    {
	return new EmptyEnumeration();
    }

    public int getPropertyCount()
    {
	return 0;
    }

    public int getPropertyType(int index)
    {
	return UNKNOWN_TYPE;
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
	return propertyValuesArray;
    }
    public void setPropertyValues(int nArgs, Object args[])
    {
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
}







