
package ircam.jmax.editors.sequence.track;

import ircam.jmax.fts.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.toolkit.*;

/**
 * The EventValue object that represents a string. */
public class StringValue extends AbstractEventValue
{
    public StringValue()
    {
	super();
    }

    /**
     * Set the named property.
     * The properties for this value are "text ", a String, and "position", an integer.
     */
    public void setProperty(String name, Object value)
    {
	if (name.equals("text"))
	     text = (String)value;
	else if (name.equals("position"))
	    position = ((Integer) value).intValue();
    }


    /**
     * Get the given property */
    public Object getProperty(String name)
    {
	if (name.equals("text"))
	    return text;
	else if (name.equals("position"))
	    return new Integer(position);
	else return UNKNOWN_PROPERTY;
	
    }
    

    /**
     * returns the ValueInfo associated to this class */
    public ValueInfo getValueInfo()
    {
	return info;
    }

    static class StringValueInfo extends AbstractValueInfo {
	/**
	 * Returns the name of this value object */
	public String getName()
	{
	    return STRING_NAME;
	}

	public Object newInstance()
	{
	    return new StringValue();
	}

    }


    /**
     * Returns its specialized renderer */
    public ObjectRenderer getRenderer()
    {
	return StringEventRenderer.getRenderer();
    }
  
    public ValueEditor getValueEditor()
    {
	return StringEditor.getInstance();
    } 

   //--- Fields

    public static final String STRING_NAME = "String";
    public static StringValueInfo info = new StringValueInfo();
    private String text = new String();    
    private int position = 0;
}

