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

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;

import ircam.jmax.toolkit.*;

/**
 * An utility class a graphic only event in a track.  
 */

public class UtilTrackEvent implements Event, Drawable
{

    public UtilTrackEvent()
    {
	time = DEFAULT_TIME;
    }

    public UtilTrackEvent(EventValue value)
    {
	time = DEFAULT_TIME;

	setValue(value);
    }

    /**
     * Get the initial time for this event */
    public double getTime()
    {
	return time;
    }
    
    /**
     * Set the initial time for this event. Use move() when the event is into a 
     * model (i.e. always for the editors, the exception are Events created on-the-fly 
     * by specific intereaction modules), in order to keep the DB consistency. */
    public void setTime(double time)   
    {
	this.time = time;
    }

    /**
     * This is the method that must be called by the editors to
     * change the initial time of an event. It takes care of
     * keeping the data base consistency */
    public void move(double time)
    {
	if (time < 0) time = 0;
	else setTime(time);
    }

    /**
     * Set the named property */
    public void setProperty(String name, Object theValue)
    {

	//int intVal;
	double doubleVal;

	if (theValue instanceof Double) 
	    {
		doubleVal = ((Double)theValue).doubleValue();
		if (name.equals("time"))
		    setTime(doubleVal);
		else  {
		    if (value != null) value.setProperty(name, theValue); //unknown Double property
		}
	    }
	else if (value != null)
	    value.setProperty(name, theValue); //unknow not-Integer property, delegate it to the value object
    }


    /**
     * Get the given property.
     * The property can be either a time or a
     * property handled by this object's value field.
     * Usually, the time property is not get with the getProperty method, 
     * but via the direct methods getTime() */
    public Object getProperty(String name)
    {
	if (name.equals("time"))
	    return new Double(time);
	else if (value != null && !value.getProperty(name).equals(EventValue.UNKNOWN_PROPERTY))
	    return value.getProperty(name); //this is not a know property, ask to the value object
	else return EventValue.DEFAULT_PROPERTY;

    }

    /**
     * Returns the value of this event */
    public EventValue getValue()
    {
	return value;
    }

    /** Set the Value corresponding to this event */
    public void setValue(EventValue value)
    {
	this.value = value;
    }

    public void setOriginal(TrackEvent evt)
    {
	trackEvent = evt;
    }
    public TrackEvent getOriginal()
    {
	return trackEvent;
    }
    
    int deltaX = 0;
    public int getDeltaX(Adapter adapter)
    {
	//return (adapter.getX(this) - adapter.getX(trackEvent));
	return deltaX;
    }
    public void setDeltaX(int dx)
    {
	deltaX = dx;
    }
    public int getDeltaY(Adapter adapter)
    {
	return (adapter.getY(this) - adapter.getY(trackEvent));
    }

    public void setLocalProperties(Event evt)
    {
	String names[] = evt.getValue().getLocalPropertyNames();
	Object localValues[] = evt.getValue().getLocalPropertyValues();
	int count = evt.getValue().getLocalPropertyCount();

	for(int i = 0; i<count; i++)
	    value.setProperty(names[i], localValues[i]);
    }

    /* --------- Drawable interface ----------*/

    public SeqObjectRenderer getRenderer()
    {
	if (value!= null)
	    return value.getRenderer();
	else return AmbitusEventRenderer.getRenderer();
	// difficult choice here:
	// the value field is not there yet, and we're asked to provide
	// a renderer for this object of unknown type.
	// AmbitusEventRenderer is choosen here because it is a renderer 
	// that is able at least to correctly show
	// a rectangle with the starting time of an event.
    }

    /*--------------  Editable interface --------------------*/
    public ValueEditor getValueEditor()
    {
	if (value != null)
	    return value.getValueInfo().newValueEditor();
	else return NullEditor.getInstance();
    }

    //--- Fields
    private double time;
    private EventValue value;
    private boolean inGroup = false;
    private TrackEvent trackEvent;

    public static double DEFAULT_TIME = 0;
}









