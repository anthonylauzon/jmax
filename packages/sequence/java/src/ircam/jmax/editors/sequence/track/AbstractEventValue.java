
package ircam.jmax.editors.sequence.track;

import ircam.jmax.fts.*;
import java.util.*;
import ircam.jmax.toolkit.*;

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
	properties.put(name, value);
    }
    
    /**
     * Get the given property */
    public Object getProperty(String name)
    {
	if (properties.containsKey(name))
	    return properties.get(name);
	else return UNKNOWN_PROPERTY; //to be overloaded
    }
    
    
    /**
     * This implementation returns an EmptyEnumeration */
    public Enumeration getPropertyNames()
    {
	return new EmptyEnumeration();
    }

    /**
     * Returns the renderer for this object */
    public ObjectRenderer getRenderer()
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
    protected Hashtable properties = new Hashtable();
}
