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
import ircam.jmax.editors.sequence.*;
import java.util.Enumeration;

/**
 * The generic model for the data contained in a Track.
 * A Track is defined as a sequence of Event objects (objects with a 
 * starting time and a duration), each one containing an EventValue.
 * Note that a track can contain EventValues of different types. 
 * A TrackDataModel is the database for a Track, and
 * it has the functionalities to add, remove and access events, as well
 * as methods to access subset of elements using time criterias
 * (IntersectionSearch, InclusionSearch methods).
 */
public interface TrackDataModel {

    /**
     * how many events in the database
     */
    public abstract int length();

    /**
     * return the time of the last event
     */
    public abstract double getMaximumTime();
    
    
    /**
     * returns an enumeration of all the events
     */
    public abstract Enumeration getEvents();

    /**
     * returns an enumeration of all the events between start and end index
     */
    public abstract Enumeration getEvents(int startIndex, int endIndex);
        
    /**
     * returns a given event 
     */
    public abstract TrackEvent getEventAt(int index);
    
    /**
     * returns the next in time event 
     */
    public abstract TrackEvent getNextEvent(Event evt);
    /**
     * returns the next in time event 
     */
    public abstract TrackEvent getPreviousEvent(double time);
    /**
     * return the index of the given event, if it exists, or the error constants
     * NO_SUCH_EVENT, EMPTY_COLLECTION 
     */
    public abstract int indexOf(Event event);
    
    public abstract Enumeration intersectionSearch(double start, double end);
    
    public abstract Enumeration inclusionSearch(double start, double end);
    
    public abstract int getFirstEventAt(double time);

    public abstract TrackEvent getLastEvent();
    
    /**
     * adds an event in the database
     * @param theEvent the TrackEvent to be added
     */
    public abstract void addEvent(TrackEvent theEvent);

    /**
     * create an event and add it in the database. 
     * callback from fts 
     * @param objId the id of the new FtsSequenceEventObject
     * @param timeTag the time tag of the new event
     * @param valueType the EventVale type for the new Event
     * @param nArgs the num of args for this value
     * @param args the Vector of arguments (Objects)
     */
    //public abstract void addNewEvent(int objId, double timeTag, String valueType, int nArgs, Object args[]);
    
    /**
     * send a addEvent message to fts
     * @param trackId the id of the track where we wont to add the new event
     * @param objId the id of the new event
     * @param time the time tag of the new event
     * @param valueType the EventVale type for the new event
     * @param nArgs the num of args for this value
     * @param args the Vector of arguments (Objects)
     */
    //public abstract void sendAddEventMessage(int trackId, int objId, float time, String valueType, int nArgs, Object args[]);

    /**
     * generic change of an event in the database.
     * Call this function to signal the parameters changing of the event, except
     * the initial time and the duration parameters. Use moveEvent and resizeEvent for that.
     * @param theEvent the TrackEvent to be changed
     * @param propertyName the name of the changed property
     * @param propertyValue the new property value
     */
    public abstract void changeEvent(TrackEvent event, String popertyName, Object propertyValue);
    
    /**
     * move an event in the database
     * @param theEvent the TrackEvent to be moved
     * @param time the new time tag
     */
    public abstract void moveEvent(TrackEvent event, double time);
    
    /**
     * deletes an event from the database
     * @param theEvent the TrackEvent to be removed
     */
    public abstract void removeEvent(TrackEvent theEvent);
    public abstract void removeEvents(Enumeration events);
    /**
     * Deletes all the events 
     */
    public abstract void removeAllEvents();
    
    /**
     * requires to be notified when the database changes
     */
    public abstract void addListener(TrackDataListener theListener);
    
    /**
     * removes the listener
     */
    public abstract void removeListener(TrackDataListener theListener);
    

    public abstract String getName();
    /**
     * Returns an Enumeration of all the types (ValueInfo) contained in this
     * model */
    public abstract Enumeration getTypes();

    public abstract ValueInfo getTypeAt(int i);

    /**
     * Returns true if this models currently contains events of the given type */
    public abstract boolean containsType(ValueInfo info);

    /*
     * Return the number of different types in this model */
    public abstract int getNumTypes();

    public abstract int getNumProperty();

    public abstract Enumeration getPropertyNames();

    public abstract void addHighlightListener(HighlightListener listener);
    public abstract void removeHighlightListener(HighlightListener listener);
    /**
     * Move all the events of the given model in this model, and
     * remove them from the original one. After this operation, the old
     * model is empty, but its content can be get back using the unmergeModel() call.
     */
    //public abstract void mergeModel(TrackDataModel model);

    /**
     * Fill the given TrackDataModel with all the event of type info
     * contained in this MultiSequence. This function could be used to
     * reverse the effect of a mergeModel call. 
     * The resulting model can have a different content if the merged model
     * have been edited in the meanwhile.
     */
    //public abstract void unmergeModel(TrackDataModel model, ValueInfo info);

    /**
     * Error code
     */
    public int EMPTY_COLLECTION = Integer.MAX_VALUE;

    /**
     * Error code
     */
    public int NO_SUCH_EVENT = Integer.MIN_VALUE;
  
}








