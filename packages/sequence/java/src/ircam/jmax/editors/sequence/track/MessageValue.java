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

/**
 * The EventValue object that represents a Integer event. Is used during score-recognition */
public class MessageValue extends AbstractEventValue
{
    public MessageValue()
    {
	super();

	setText("", null, null);
	properties.put("integer", new Integer(0));
	properties.put("open", new Boolean(true));
    }

    Object message = "";
    Object integer;
    public void setProperty(String name, Object value)
    {
	if(name.equals("message"))
	    message = value;
	else
	    if(name.equals("integer"))
		integer = value;
	
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
		return super.getProperty(name);
    }

    public void edit(int x, int y, int modifiers, Event evt, SequenceGraphicContext gc)
    {
	if(x-gc.getAdapter().getX(evt) <= MessageEventRenderer.BUTTON_WIDTH)
	    {//doubleClick on the button then open or close the object
		int size;
		boolean open = ((Boolean)getProperty("open")).booleanValue();
		setProperty("open", new Boolean(!open));
		
		if(open) size = gc.getAdapter().getInvWidth(MessageEventRenderer.BUTTON_WIDTH);
		else 
		    {
			FontMetrics fm = gc.getGraphicDestination().getFontMetrics(MessageEventRenderer.stringFont);
			String mess = (String) getProperty("message");
			int evtLenght;
			if(!mess.equals(""))
			    evtLenght = fm.stringWidth(mess)+4+MessageEventRenderer.BUTTON_WIDTH;
			else
			    evtLenght = MessageEventRenderer.DEFAULT_WIDTH;

			size = gc.getAdapter().getInvWidth(evtLenght);
		    }
		setProperty("duration", new Integer(size));
	    }
	else
	    {
		//edit the text in the object SPORCOOOOOOOOOOOOOOOOOOOOOO!!!!!!!
		((MessageTrackEditor)gc.getGraphicDestination()).doEdit(evt);
	    }
    }

    public void setText(String text, TrackEvent evt, SequenceGraphicContext gc)
    {	
	int width, evtLenght;
	if(gc!=null)
	    {
		FontMetrics fm = gc.getGraphicDestination().getFontMetrics(MessageEventRenderer.stringFont);		
		if(!text.equals(""))
		    evtLenght = fm.stringWidth(text)+4+MessageEventRenderer.BUTTON_WIDTH; 
		else
		    evtLenght = MessageEventRenderer.DEFAULT_WIDTH;

		width = gc.getAdapter().getInvWidth(evtLenght);
	    }
	else 
	    width = DEFAULT_WIDTH;

	setProperty("message", text);
	setProperty("duration", new Integer(width));

	if(evt!=null)
	    evt.sendSetMessage(info.getName(), getPropertyCount(), getPropertyValues());
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
 
	String defNamesArray[] = {"message", "integer"};
	int defPropertyCount = 2;
    }

    /**
     * Returns its specialized renderer (an AmbitusEventRenderer) */
    public ObjectRenderer getRenderer()
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

    //--- Fields
    public static final String fs = File.separator;
    public static final String MESSAGE_NAME = "messevt";
    public static final String MESSAGE_PUBLIC_NAME = "message";
    public static MessageValueInfo info = new MessageValueInfo();
    public static final int DEFAULT_WIDTH = 290;
    static String path;
    public static ImageIcon MESSAGE_ICON;
    static String nameArray[] = {"message", "integer"};
    static String localNameArray[] = {"duration", "open"};
    static int propertyCount = 2;
    static int localPropertyCount = 2;

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




