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
import ircam.jmax.fts.*;
import javax.swing.undo.*;
import ircam.jmax.toolkit.*;

/**
 * The class representing an event in a track. These objects have a time and duration, 
 * and a value (a FtsRemoteData).
 */

public class TrackEvent extends FtsRemoteData implements Event, Drawable, UndoableData, Cloneable
{

    public TrackEvent()
    {
	time = DEFAULT_TIME;
	duration = DEFAULT_DURATION;

    }


    public void call(int i, FtsStream stream)
    {
	//to be implemented 
    }

    public TrackEvent(TrackDataModel model, EventValue value)
    {
	time = DEFAULT_TIME;
	duration = DEFAULT_DURATION;

	itsTrackDataModel = model;
    }


    /**
     * Sets the data model this event belongs to */
    public void setDataModel(TrackDataModel model)
    {
	itsTrackDataModel = model;
    }


    /**
     * Gets the data model this event belongs to */
    public TrackDataModel getDataModel()
    {
	return itsTrackDataModel;
    }


    public TrackEvent(EventValue value)
    {
	time = DEFAULT_TIME;
	duration = DEFAULT_DURATION;

	setValue(value);
    }

    /**
     * Get the initial time for this event */
    public int getTime()
    {
	return time;
    }
    
    /**
     * Set the initial time for this event. Use move() when the event is into a 
     * model (i.e. always for the editors, the exception are Events created on-the-fly 
     * by specific intereaction modules), in order to keep the DB consistency. */
    public void setTime(int time)
    {
	this.time = time;
    }

    /**
     * This is the method that must be called by the editors to
     * change the initial time of an event. It takes care of
     * keeping the data base consistency */
    public void move(int time)
    {
	if (time < 0) time = 0;
	if (itsTrackDataModel != null)
	    {
		if (((UndoableData) itsTrackDataModel).isInGroup())
		    ((UndoableData) itsTrackDataModel).postEdit(new UndoableMove(this));
		
		itsTrackDataModel.moveEvent(this, time);
	    }
	else setTime(time);
    }

    /**
     * Get the duration of this event */
    public int getDuration()
    {
	return duration;
    }

    /**
     * Set the duration of this event */
    public final void setDuration(int duration) 
    {
	
	if (duration < 0) duration = 0;
	if (itsTrackDataModel != null)
	    {
		if (((UndoableData) itsTrackDataModel).isInGroup())
		    ((UndoableData) itsTrackDataModel).postEdit(new UndoableEventTransformation(this));
	    }
	
	this.duration = duration;
	
	if (itsTrackDataModel != null)
	    {
		itsTrackDataModel.changeEvent(this);
	    }
    }
    

    /**
     * Set the named property */
    public void setProperty(String name, Object theValue)
    {

	int intVal;

	if (itsTrackDataModel != null)
	    {
		if (((UndoableData) itsTrackDataModel).isInGroup())
		    ((UndoableData) itsTrackDataModel).postEdit(new UndoableEventTransformation(this));
	    }
	
	
	if (theValue instanceof Integer) 
	    {
		intVal = ((Integer)theValue).intValue();

		if (name.equals("time"))
		    setTime(intVal);
		else if (name.equals("duration"))
		    setDuration(intVal);
		else  {
		    if (value != null) value.setProperty(name, theValue); //unknown Integer property
		}
	    }
	else if (value != null)
	    value.setProperty(name, theValue); //unknow not-Integer property, delegate it to the value object
	

	if (itsTrackDataModel != null)
	    {
		itsTrackDataModel.changeEvent(this);
	    }

    }


    /**
     * Get the given property.
     * The property can be either a time, duration, or a
     * property handled by this object's value field.
     * Usually, the time and duration properties are not get with the getProperty method, 
     * but via the direct methods getTime() and getDuration() */
    public Object getProperty(String name)
    {
	if (name.equals("time"))
	    return new Integer(time);
	else if (name.equals("duration"))
	    return new Integer(duration);
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



    /* --------- Drawable interface ----------*/

    public ObjectRenderer getRenderer()
    {
	if (value!= null)
	    return value.getRenderer();
	else return AmbitusEventRenderer.getRenderer();
	// difficult choice here:
	// the value field is not there yet, and we're asked to provide
	// a renderer for this object of unknown type.
	// AmbitusEventRenderer is choosen here because it is a renderer 
	// that is able at least to correctly show
	// a rectangle with the starting time and duration of an event.
    }

    /* --------- Undoable data interface ----------*/

    /**
     * Tells the model to start an undo section */
    public  void beginUpdate()  
    {
	((UndoableData) itsTrackDataModel).beginUpdate();
	inGroup = true;
    }

    
    /**
     * posts an undo edit in the buffers */
    public  void postEdit(UndoableEdit e)
    {
    }
    
    /**
     * Signal that the undoable section ended */
    public void endUpdate() 
    {
	((UndoableData) itsTrackDataModel).endUpdate();
	inGroup = false;
    }

    /**
     * Return true if an undoable section have been opened, and not closed yet*/
    public boolean isInGroup() 
    {
	return inGroup;
    }

    /**
     * ask to undo the section */
    public  void undo()
    {
    }
    
    /**
     * ask to redo the section */
    public  void redo()
    {
    }
    

    /*--------------  Editable interface --------------------*/
    public ValueEditor getValueEditor()
    {
	if (value != null)
	    return value.getValueInfo().newValueEditor();
	else return NullEditor.getInstance();
    }

    public TrackEvent duplicate () throws CloneNotSupportedException
    {
	return (TrackEvent) clone();
    }


    //--- Fields
    private int time;
    private int duration;
    private EventValue value;
    private boolean inGroup = false;

    public static int DEFAULT_TIME = 0;
    public static int DEFAULT_DURATION = 100;

    private TrackDataModel itsTrackDataModel;
}
