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
import java.awt.*;
import java.awt.datatransfer.*;

/**
 * The EventValue object that represents a Integer event. Is used during score-recognition */
public class MessageValue extends AbstractEventValue
{
    Object message = "";
    Object integer = new Integer(0);
    Object open = new Boolean(true);

    public MessageValue()
    {
	super();

	setText("", null, null);
	properties.put("integer", integer);
	properties.put("open", open);
    }

    public void setProperty(String name, Object value)
    {
	if(name.equals("message"))
	    message = value;
	else
	    if(name.equals("integer"))
		integer = value;
	else
	    if(name.equals("open"))
		open = value;

	super.setProperty( name, value);
    }

    public Object getProperty(String name)
    {
	if(name.equals("message"))
	    return message;
	else
	    if(name.equals("integer"))
		return integer;
	    else
		if(name.equals("open"))
		    return open;
	    else
		return super.getProperty(name);
    }

    public void edit(int x, int y, int modifiers, Event evt, SequenceGraphicContext gc)
    {
	if(isOnTheButton(evt, x, gc))
	    {
		int width, height;
		boolean open = ((Boolean)getProperty("open")).booleanValue();
		setProperty("open", new Boolean(!open));
		
		String text = ((MessageAdapter)gc.getAdapter()).getLabel(evt);
		
		if(!text.equals(""))
		    height = TextRenderer.getRenderer().getTextHeight(text, gc);
		else
		    height = DEFAULT_HEIGHT;
		
		if(open) 
		    width = ((MessageAdapter)gc.getAdapter()).getInvWidth(MessageEventRenderer.BUTTON_WIDTH);
		else 
		    {
			int evtLenght;
		
			if(!text.equals(""))
			    evtLenght = TextRenderer.getRenderer().getTextWidth(text, gc)+4+MessageEventRenderer.BUTTON_WIDTH; 
			else
			    evtLenght = MessageEventRenderer.DEFAULT_WIDTH;
			
			width = ((MessageAdapter)gc.getAdapter()).getInvWidth(evtLenght);
		    }
		//setProperty("duration", new Integer(width));
		setProperty("duration", new Double(width));
		setProperty("height", new Integer(height));
	    }
    }

    public boolean isOnTheButton(Event evt, int x, SequenceGraphicContext gc)
    {
    	return (x-gc.getAdapter().getX(evt) <= MessageEventRenderer.BUTTON_WIDTH);

    }

    public void setText(String text, TrackEvent evt, SequenceGraphicContext gc)
    {	
	int width, evtLenght, height;

	if(gc!=null)
	    {
		if(!text.equals(""))
		    {
			evtLenght = TextRenderer.getRenderer().getTextWidth(text, gc)+4+MessageEventRenderer.BUTTON_WIDTH; 
			height = TextRenderer.getRenderer().getTextHeight(text, gc);
		    }		
		else
		    {
			evtLenght = MessageEventRenderer.DEFAULT_WIDTH;
			height = DEFAULT_HEIGHT;
		    }

		width = ((MessageAdapter)gc.getAdapter()).getInvWidth(evtLenght);
	    }
	else 
	    {
		width = DEFAULT_WIDTH;
		height = DEFAULT_HEIGHT;
	    }

	if(evt != null)
	    {
		//for the undo/redo
		evt.setProperty("message", text);
		evt.setProperty("duration", new Double(width));
		evt.setProperty("height", new Integer(height));

		evt.sendSetMessage(MESSAGE_NAME, getPropertyCount(), getPropertyValues());
	    }
	else
	    {
		setProperty("message", text);
		
		//setProperty("duration", new Integer(width));
		setProperty("duration", new Double(width));
		
		setProperty("height", new Integer(height));
	    }
    }

    public void updateHeight(TrackEvent evt, SequenceGraphicContext gc)
    {
	int height;
	String text = (String) message;

	if(!text.equals(""))
	    height = TextRenderer.getRenderer().getTextHeight(text, gc);
	else
	    height = DEFAULT_HEIGHT;

	setProperty("height", new Integer(height));
    }

    public void updateLength(TrackEvent evt, SequenceGraphicContext gc)
    {
	int width, evtLenght;

	String text = (String) message;
	boolean open = ((Boolean)getProperty("open")).booleanValue();
	if(open)
	    {
		if(!text.equals(""))
		    evtLenght = TextRenderer.getRenderer().getTextWidth(text, gc)+4+MessageEventRenderer.BUTTON_WIDTH; 
		else
		    evtLenght = MessageEventRenderer.DEFAULT_WIDTH;
	    }
	else
	    evtLenght = MessageEventRenderer.BUTTON_WIDTH;

	 width = ((MessageAdapter)gc.getAdapter()).getInvWidth(evtLenght);
	
	 //setProperty("duration", new Integer(width));
	 setProperty("duration", new Double(width));
    }

    public ValueInfo getValueInfo() 
    {
	return info;
    }

    static class MessageValueInfo extends AbstractValueInfo {
	/**
	 * Returns the name of this value object */
	public String getName()
	{
	    return MESSAGE_NAME;
	}

	public String getPublicName()
	{
	    return MESSAGE_PUBLIC_NAME;
	}

	public ImageIcon getIcon()
	{
	    return MESSAGE_ICON;
	}

	public Object newInstance()
	{
	    return new MessageValue();
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
	    return MessageValueDataFlavor.getInstance();
	}

	public Class getPropertyType(int index)
	{
	    if(index < defPropertyCount)
		return propertyTypesArray[index];
	    else
		return Integer.class;
	}
	
	
	String defNamesArray[] = {"integer", "message"};
	Class propertyTypesArray[] = {Integer.class, String.class};
	int defPropertyCount = 2;
    }

    /**
     * Returns its specialized renderer (an AmbitusEventRenderer) */
    public SeqObjectRenderer getRenderer()
    {
	return MessageEventRenderer.getRenderer();
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

    public String[] getLocalPropertyNames()
    {
	return localNameArray;
    }
    
    public int getLocalPropertyCount()
    {
	return localPropertyCount;
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

    public Object[] getLocalPropertyValues()
    {
	for(int i = 0; i<localPropertyCount; i++)
	    propertyValuesArray[i] = getProperty(localNameArray[i]);
	
	return propertyValuesArray;
    }

    public void setLocalPropertyValues(int nArgs, Object args[])
    {
	for(int i = 0; i<nArgs; i++)
	    setProperty(localNameArray[i], args[i]);
    }

    public boolean samePropertyValues(Object args[])
    {
      return (((String)getProperty("message")).equals((String)args[1]) &&
	      (((Integer)getProperty("integer")).intValue() == ((Integer)args[0]).intValue()));
    }

    //--- Fields
    public static final String fs = File.separator;
    public static final String MESSAGE_NAME = "message";
    public static final String MESSAGE_PUBLIC_NAME = "message";
    public static MessageValueInfo info = new MessageValueInfo();
    public static final int DEFAULT_WIDTH = 290;
    public static final int DEFAULT_HEIGHT = 13;
    static String path;
    public static ImageIcon MESSAGE_ICON;
    static String nameArray[] = {"integer", "message"};
    static int propertyTypes[] = {INTEGER_TYPE, STRING_TYPE};
    static String localNameArray[] = {"duration", "open", "height"};
    static int propertyCount = 2;
    static int localPropertyCount = 3;

    static 
    {
	try
	    {
		path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+fs+"images"+fs;
	    }
	catch(FileNotFoundException e){
	    //System.err.println("Couldn't locate sequence images");
	    path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
	}
	MESSAGE_ICON = new ImageIcon(path+"message.gif");
    }
}
